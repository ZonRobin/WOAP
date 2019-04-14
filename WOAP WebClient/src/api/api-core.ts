import * as SocketIOClient from "socket.io-client";
import {Comparable} from "../comparable";
import {ListenerLibrary} from "../listener-library";
import {ConfigurationManager} from "../configuration-manager";
import OSCMessage from "../osc/osc-message";
import OSCBundle from "../osc/osc-bundle";
import OSCMapper from "../osc/osc-mapper";
import OSCElement from "../osc/osc-element";
import ValidatableOSCMessage from "./messages/validatable-osc-message";
import ValidatableOSCMessageWithId from "./messages/validatable-osc-message-with-id";
import APIAsyncMessageReceiver from "./api-async-message-receiver";
import Socket = SocketIOClient.Socket;
import {ArrayExtended} from "../array-extended";

export interface MessageReceiver {
    oscMessageReceived(message: OSCMessage, messageType: number): void;
    oscBundleReceived(bundle: OSCBundle, messageType: number): void;
}

export type AddressWithTypeId = [string, number];

export class MessageReceiverWrapper implements Comparable {
    public receiver: MessageReceiver;
    public messageType: number;
    public deleteOnReceive: boolean;

    constructor(receiver: MessageReceiver, messageType: number = 0, deleteOnReceive: boolean = false) {
        this.receiver = receiver;
        this.messageType = messageType;
        this.deleteOnReceive = deleteOnReceive;
    }

    public equals(other: Comparable): boolean {
        if (other instanceof MessageReceiverWrapper) {
            return this.receiver === other.receiver;
        }
        else {
            return false;
        }
    }
}

export interface APICoreConnectionListener {
    apiCoreConnected(): void;
    apiCoreDisconnected(): void;
}

export default class APICore {
    protected socket: Socket;

    protected requestReceivers: ListenerLibrary<MessageReceiverWrapper>;

    protected asyncReceiver: APIAsyncMessageReceiver;

    protected connectionListeners: ArrayExtended<APICoreConnectionListener> = new ArrayExtended();

    protected disconnectMessageSent: boolean = false;

    protected serverURL: string;

    constructor(serverURL: string) {
        this.requestReceivers = new ListenerLibrary<MessageReceiverWrapper>();
        this.asyncReceiver = new APIAsyncMessageReceiver(this);
        this.serverURL = serverURL;
    }

    public initializeConnection(): this {
        this.socket = SocketIOClient.connect(this.serverURL, {
            transports: ["websocket"],
            reconnection: true,
            reconnectionDelay: 2500,
            reconnectionDelayMax: 5000,
            reconnectionAttempts: Infinity
        });
        this.socket.on("connect", this.socketConnectedCallback.bind(this));
        this.socket.on("connect_error", this.socketErrorCallback.bind(this));
        this.socket.on("data", this.messageReceivedCallback.bind(this));

        return this;
    }

    public sendMessage(message: ValidatableOSCMessage): this {
        if (ConfigurationManager.isLogAPICallsToConsole()) {
            console.log("Sending message: " + message.toOSCMessage().toString());
        }

        this.socket.emit("data", message.toOSCMessage().toBuffer());
        return this;
    }

    public sendBundle(bundle: OSCBundle): this {
        if (ConfigurationManager.isLogAPICallsToConsole()) {
            console.log("Sending bundle: " + bundle.toString());
        }

        this.socket.emit("data", bundle.toBuffer());
        return this;
    }

    public request(message: ValidatableOSCMessageWithId, responseAddress: string): Promise<OSCMessage> {
        return new Promise<OSCMessage>(
            (resolve: (value?: OSCMessage) => void, reject: (reason?: any) => void) => {
                this.asyncReceiver.waitForMessage(responseAddress, message.getMessageId(), resolve, reject);
                this.sendMessage(message);
            });
    }

    public addListener(method: string, listener: MessageReceiver, messageTypeId: number = 0, deleteOnReceive: boolean = false): this {
        this.requestReceivers.addListener(method, new MessageReceiverWrapper(listener, messageTypeId, deleteOnReceive));
        return this;
    }

    public addOneTimeListener(method: string, listener: MessageReceiver, messageTypeId: number = 0): this {
        return this.addListener(method, listener, messageTypeId, true);
    }

    public removeListener(method: string, listener: MessageReceiver, messageTypeId: number = 0): this {
        if (ConfigurationManager.isDebug()) {
            if (method.length === 0 || method[0] !== "/") {
                console.error("Wrong method in addListener " + method);
            }
        }

        this.requestReceivers.removeListener(method, new MessageReceiverWrapper(listener, messageTypeId));
        return this;
    }

    public addListenerBatch(listener: MessageReceiver, messages: AddressWithTypeId[]): void {
        for (let message of messages) {
            this.addListener(message[0], listener, message[1]);
        }
    }

    public removeListenerBatch(listener: MessageReceiver, messages: AddressWithTypeId[]): void {
        for (let message of messages) {
            this.removeListener(message[0], listener, message[1]);
        }
    }

    public addConnectionListener(listener: APICoreConnectionListener): this {
        this.connectionListeners.add(listener);
        return this;
    }

    public removeConnectionListener(listener: APICoreConnectionListener): this {
        this.connectionListeners.remove(listener);
        return this;
    }

    protected socketConnectedCallback(): void {
        this.disconnectMessageSent = false;
        this.connectionListeners.forEach((listener) => listener.apiCoreConnected());

        if (ConfigurationManager.isDebug()) {
            console.log("Connected to web control");
        }
    }

    protected socketErrorCallback(): void {
        if (!this.disconnectMessageSent) {
            this.disconnectMessageSent = true;
            this.connectionListeners.forEach((listener) => listener.apiCoreDisconnected());
        }

        if (ConfigurationManager.isDebug()) {
            console.log("Connection error");
        }
    }

    protected messageReceivedCallback(message: Buffer): void {
        if (ConfigurationManager.isLogAPICallsToConsole()) {
            console.log("Received message: " + OSCMapper.fromBuffer(message).toString());
        }

        this.dispatchOSCElementToReceivers(OSCMapper.fromBuffer(message));
    }

    protected dispatchOSCElementToReceivers(element: OSCElement) {
        if (element.isMessage()) {
            let listeners = this.requestReceivers.getListenersOrNull((element as OSCMessage).address);
            if (listeners !== null) {

                let willDeleteListener = false;
                listeners.forEach((listener) => {
                    if (listener.deleteOnReceive) {
                        willDeleteListener = true;
                    }

                    listener.receiver.oscMessageReceived(element as OSCMessage, listener.messageType);
                });

                if (willDeleteListener) {
                    let listenersLength = listeners.getLength();
                    for (let i = listenersLength - 1; i >= 0; --i) {
                        if (listeners.get(i).deleteOnReceive) {
                            listeners.removeAtIndex(i);
                        }
                    }
                }
            }
        }
        else {
            for (let subElement of (element as OSCBundle).elements) {
                this.dispatchOSCElementToReceivers(subElement);
            }
        }
    }
}


