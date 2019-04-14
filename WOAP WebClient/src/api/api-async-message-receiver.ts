import {default as APICore, MessageReceiver} from "./api-core";
import OSCMessage from "../osc/osc-message";
import OSCBundle from "../osc/osc-bundle";
import ValidatableOSCMessageWithId from "./messages/validatable-osc-message-with-id";
import {ConfigurationManager} from "../configuration-manager";
import {Utility} from "../utility";
import {ApiCoreMessageRejectedMessage} from "./messages/app-messages";
import App from "../app";

export default class APIAsyncMessageReceiver implements MessageReceiver {
    protected tasks: Array<APIAsyncMessageReceiverTask> = [];
    protected core: APICore;

    protected static readonly MessageTypeId = {
        MessageRejected: -100
    };

    constructor(core: APICore) {
        this.core = core;
        core.addListener(ApiCoreMessageRejectedMessage.getAddress(), this, APIAsyncMessageReceiver.MessageTypeId.MessageRejected);
    }

    public waitForMessage(address: string, messageId: number, resolve: (value?: OSCMessage) => void, reject: (reason?: any) => void): void {
        if (ConfigurationManager.isDebug()) {
            if (messageId === 0) {
                console.error("Can\"t wait for message with ID 0");
            }
        }

        let task = new APIAsyncMessageReceiverTask(this, address, messageId, resolve, reject);
        this.core.addOneTimeListener(address, this, messageId);

        this.tasks[messageId] = task;
    }

    public taskTimeOuted(task: APIAsyncMessageReceiverTask): void {
        if (ConfigurationManager.isDebug()) {
            console.error("Waiting for request time-outed");
            console.error(task);
        }
        delete this.tasks[task.messageId];
    }

    protected handleMessageRejectedMessage(message: OSCMessage): void {
        let msg = new ApiCoreMessageRejectedMessage(message);
        if (!msg.isValid())
            return;

        let messageId = msg.getRejectedMessageId();
        if (messageId !== -1) {
            if (!Utility.isUndefined(this.tasks[messageId])) {
                this.tasks[messageId].reject(msg.getResponseCode());
                delete this.tasks[messageId];
            } else {
                if (ConfigurationManager.isDebug()) {
                    console.error("Got reject message for non-existing task (" + messageId + ")");
                    console.error(message);
                    return;
                }
            }
        }

        App.showGeneralErrorBoxWithDebugMessage("Received message rejection " + msg.getResponseString() + " for message " + msg.getMessageDebugInfo());
    }

    public oscMessageReceived(message: OSCMessage, messageId: number): void {
        if (messageId === APIAsyncMessageReceiver.MessageTypeId.MessageRejected) {
            this.handleMessageRejectedMessage(message);
            return;
        }

        if (!ValidatableOSCMessageWithId.doesMessageHaveId(message)) {
            if (ConfigurationManager.isDebug()) {
                console.error("This message should have ID: ", message.toString());
            }
            return;
        }

        if (messageId !== ValidatableOSCMessageWithId.getMessageId(message)) {
            return;
            /* This message will be processed by other oscMessageReceived callback */
        }

        if (!Utility.isUndefined(this.tasks[messageId])) {
            this.tasks[messageId].resolve(message);
            delete this.tasks[messageId];
        } else {
            if (ConfigurationManager.isDebug()) {
                console.error("Got message for non-existing task (" + messageId + ")");
                console.error(message);
            }
        }
    }

    public oscBundleReceived(bundle: OSCBundle, taskId: number): void {
        throw new Error("Not supported");
    }
}

class APIAsyncMessageReceiverTask {
    public messageAddress: string;

    public messageId: number;

    public resolveCallback: (value?: OSCMessage) => void;

    public rejectCallback: (reason?: any) => void;

    public rejectTimeout: number = null;

    public receiver: APIAsyncMessageReceiver;

    constructor(receiver: APIAsyncMessageReceiver, messageAddress: string, messageId: number, resolve: (value?: OSCMessage) => void, reject: (reason?: any) => void) {
        this.receiver = receiver;
        this.messageAddress = messageAddress;
        this.messageId = messageId;
        this.resolveCallback = resolve;
        this.rejectCallback = reject;
        this.rejectTimeout = setTimeout(this.timeoutCallback.bind(this), ConfigurationManager.getAPIRequestTimeout());
    }

    public resolve(message: OSCMessage): void {
        clearTimeout(this.rejectTimeout);
        this.resolveCallback(message);
    }

    public reject(responseCode: number): void {
        clearTimeout(this.rejectTimeout);
        this.rejectCallback(responseCode);
    }

    protected timeoutCallback(): void {
        this.rejectCallback(APIAsyncMessageRejectReason.Timeout);
        this.receiver.taskTimeOuted(this);
    }
}

export enum APIAsyncMessageRejectReason {
    Timeout
}