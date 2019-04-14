import * as dgram from "dgram";
import SocketsDatagramSocket from "./datagram-socket";

export interface SocketsDatagramProxyListener {
    /**
     * Method triggered when message from proxy is received
     *
     * @param {Buffer} message
     */
    messageFromDatagramProxyReceived(message: Buffer): void;
}

export default class SocketsDatagramProxy extends SocketsDatagramSocket {
    /**
     * @type {SocketsDatagramProxyListener}
     */
    protected listener: SocketsDatagramProxyListener = null;

    /**
     * @param {SocketsDatagramProxyListener} listener
     * @param {number} sendingPort
     * @param {string} sendingAddress
     */
    constructor(listener: SocketsDatagramProxyListener, sendingPort?: number, sendingAddress?: string) {
        super(sendingPort, sendingAddress);
        this.listener = listener;
    }

    /**
     * Callback triggered when message is received
     *
     * @param {Buffer} message
     * @param {dgram.RemoteInfo} info
     */
    protected messageReceivedCallback(message: Buffer, info: dgram.RemoteInfo): void {
        this.listener.messageFromDatagramProxyReceived(message);
    }
}
