import * as dgram from "dgram";
import {ConfigurationManager} from "../configuration-manager";

export default abstract class SocketsDatagramSocket {
    /**
     * Socket listener
     *
     * @type {Socket}
     */
    socket: dgram.Socket = null;

    /**
     * Port to send messages to
     *
     * @type {number}
     */
    sendingPort: number;

    /**
     * Address to send message to
     *
     * @type {string}
     */
    sendingAddress: string;

    /**
     * @param {number} sendingPort
     * @param {string} sendingAddress
     */
    constructor(sendingPort?: number, sendingAddress?: string) {
        this.sendingAddress = sendingAddress;
        this.sendingPort = sendingPort;
    }

    /**
     * Callback triggered when message is received
     *
     * @param {Buffer} message
     * @param {dgram.RemoteInfo} info
     */
    protected abstract messageReceivedCallback(message: Buffer, info: dgram.RemoteInfo): void;

    /**
     * Bind socket to specific port or random one
     *
     * @param {number} port
     * @param {string} address
     * @param {() => void} callback
     */
    public bind(port: number = 0, address?: string, callback?: () => void): void {
        this.socket = dgram
            .createSocket('udp4')
            .on('message', this.messageReceivedCallback.bind(this))
            .on('close', this.closeCallback.bind(this))
            .on('error', this.errorCallback.bind(this));

        this.socket.bind(port, address, callback);
    }

    /**
     * Close socket
     */
    public close(): void {
        if (this.socket !== null) {
            this.socket.close();
            this.socket = null;
        }
    }

    /**
     * @returns {string}
     */
    public getReceivingAddress(): string {
        return this.socket.address().address;
    }

    /**
     * @returns {number}
     */
    public getReceivingPort(): number {
        return this.socket.address().port;
    }

    /**
     * @returns {string}
     */
    public getSendingAddress(): string {
        return this.sendingAddress;
    }

    /**
     * @returns {number}
     */
    public getSendingPort(): number {
        return this.sendingPort;
    }

    /**
     * @param {number} newPort
     */
    public setSendingPort(newPort: number): void {
        this.sendingPort = newPort;
    }

    /**
     * @param {string} newAddress
     */
    public setSendingAddress(newAddress: string): void {
        this.sendingAddress = newAddress;
    }

    /**
     * Send raw data to endpoint
     *
     * @param {Buffer} message
     */
    public sendMessage(message: Buffer) {
        if (this.socket !== null && this.sendingPort > 0) {
            this.socket.send(message, this.sendingPort, this.sendingAddress);
        }
    }

    /**
     * Callback triggered when error is spawned
     *
     * @param {Error} error
     */
    protected errorCallback(error: Error): void {
        if (ConfigurationManager.isDevelopment()) {
            console.log("Unhandled datagram socket error " + error.message);
            throw error;
        }
        else {
            this.close();
        }
    }

    /**
     * Called on close event
     */
    protected closeCallback(): void {

    }
}
