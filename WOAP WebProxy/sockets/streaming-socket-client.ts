import * as Net from "net";
import {SocketsHelper} from "./helper";
import {ConfigurationManager} from "../configuration-manager";
import ErrnoException = NodeJS.ErrnoException;

export default abstract class SocketsStreamingSocketClient implements SocketsHelper.DecoderListener {
    /**
     * Socket connected to server
     * @type {Socket}
     */
    protected socket: Net.Socket = null;

    /**
     * @type {boolean}
     */
    protected autoReconnect: boolean;

    /**
     * @type {number}
     */
    protected lastPort: number;

    /**
     * @param {boolean} autoReconnect
     */
    constructor(autoReconnect: boolean = true) {
        this.autoReconnect = autoReconnect;
    }

    /**
     * Called on each message received
     *
     * @param {Buffer} message
     */
    public abstract messageReceived(message: Buffer): void;

    /**
     * Listen to port
     *
     * @param {number} port
     */
    public listen(port: number): void {
        this.lastPort = port;
        this.socket = Net
            .createConnection({port: port, host: '127.0.0.1'}, this.connectedCallback.bind(this))
            .on('data', this.dataReceivedCallback.bind(this))
            .on('end', this.endCallback.bind(this))
            .on('error', this.errorCallback.bind(this));
    }

    /**
     * @param {boolean} requestedByUser
     * Disconnect socket
     */
    public disconnect(requestedByUser: boolean = false): void {
        if (this.socket !== null) {
            this.socket.destroy();
            this.socket = null;
            this.disconnectedCallback(requestedByUser);
        }
    }

    /**
     * Disconnect socket
     */
    public reconnect(): void {
        if (this.socket !== null) {
            this.socket.destroy();
            this.socket = null;
            this.disconnectedCallback();
            this.listen(this.lastPort);
        }
    }

    /**
     * Send raw data to client
     *
     * @param {Buffer} message
     */
    public sendMessage(message: Buffer) {
        if (this.socket !== null) {
            this.socket.write(SocketsHelper.encodeBuffer(message));
        }
    }

    /**
     * Send encoded data to client
     *
     * @param {Buffer} message
     */
    public sendEncodedMessage(message: Buffer) {
        if (this.socket !== null) {
            this.socket.write(message);
        }
    }

    /**
     * Callback triggered when client is connected to server
     */
    protected connectedCallback(): void {
    }

    /**
     * @param {boolean} requestedByUser
     * Callback triggered when client is disconnected from server
     */
    protected disconnectedCallback(requestedByUser: boolean = false): void {
    }


    /**
     * Callback triggered when message is received
     *
     * @param {Buffer} data
     */
    protected dataReceivedCallback(data: Buffer): void {
        SocketsHelper.decodeBufferWithCallback(data, this);
    }

    /**
     * Called on end event
     */
    protected endCallback(): void {
        this.autoReconnect ? this.reconnect() : this.disconnect();
    }

    /**
     * Callback triggered when error is spawned
     *
     * @param {ErrnoException} error
     */
    protected errorCallback(error: ErrnoException): void {
        switch (error.code) {
            case 'ECONNRESET':
            case 'ECONNABORTED':
            case 'EPIPE': {
                this.autoReconnect ? this.reconnect() : this.disconnect();
                break;
            }
            case 'ECONNREFUSED': {
                this.listen(this.lastPort);
                break;
            }
            default: {
                if (ConfigurationManager.isDevelopment()) {
                    console.error("Unhandled socket error " + error.code);
                    throw error;
                }
                else {
                    this.autoReconnect ? this.reconnect() : this.disconnect();
                }
            }
        }
    }
}
