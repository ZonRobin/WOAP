import * as SocketIO from "socket.io";
import {ConfigurationManager} from "./configuration-manager";
import {ApiCoreErrorMessage, ApiCoreReadyMessage} from "./messages/app-messages";
import SocketsStreamingSocketClient from "./sockets/streaming-socket-client";
import ValidatableOSCMessage from "./messages/validatable-osc-message";
import SocketIOSocket = SocketIO.Socket;
import ErrnoException = NodeJS.ErrnoException;

interface SocketIODataStructure {
    type: string;
    data: any;
}

/**
 * Client for websocket server
 */
class WebProxyClient extends SocketsStreamingSocketClient {
    /**
     *  Socket connected to web client
     *  @type {SocketIOSocket}
     */
    protected webClient: SocketIOSocket = null;

    /**
     * @type {WebProxyServer}
     */
    protected server: WebProxyServer = null;

    /**
     * @param {WebProxyServer} server
     * @param {Socket} webClient
     */
    constructor(server: WebProxyServer, webClient: SocketIOSocket) {
        super();
        this.server = server;
        this.webClient = webClient;

        this.webClient.on('data', this.webClientMessageReceivedCallback.bind(this));
        this.webClient.on('disconnect', () => this.server.webClientLeft(this));
        this.webClient.on('end', () => this.server.webClientLeft(this));
        this.webClient.on('error', this.webClientErrorCallback.bind(this));
    }

    public getWebClient(): SocketIOSocket {
        return this.webClient;
    }

    /**
     * Initialize connection
     */
    public initialize(): void {
        this.listen(5020);
    }

    /**
     * Terminate connection, disconnect web client and host
     */
    public terminate(): void {
        this.disconnect();
        this.webClient.disconnect();
    }

    /**
     * Callback triggered when error occurs
     * @param {ErrnoException} error
     */
    public webClientErrorCallback(error: ErrnoException): void {
        if (ConfigurationManager.isDevelopment()) {
            console.log("Unhandled socket web client error " + error.code);
            throw error;
        } else {
            this.server.webClientLeft(this);
        }
    }

    /**
     * @param {ValidatableOSCMessage} message
     */
    public sendToWebClient(message: ValidatableOSCMessage): void {
        this.webClient.emit('data', message.toOSCMessage().toBuffer());
    }

    /**
     * Message from host to web
     * @param {Buffer} message
     */
    public messageReceived(message: Buffer): void {
        this.webClient.emit('data', message);
    }

    /**
     * Callback triggered when client is connected to server
     */
    protected connectedCallback(): void {
        this.sendToWebClient(new ApiCoreReadyMessage());
    }

    /**
     * Message from web to host
     * @param {SocketIODataStructure} message
     */
    protected webClientMessageReceivedCallback(message: SocketIODataStructure): void {
        if (message.type === 'Buffer') {
            this.sendMessage(new Buffer(message.data));
        } else {
            if (ConfigurationManager.isDevelopment()) {
                throw new Error('Unknown message type ' + message.type);
            }
        }
    }

    /**
     * Host disconnected
     */
    protected disconnectedCallback(requestedByUser: boolean = false): void {
        if (!requestedByUser)
            this.sendToWebClient(new ApiCoreErrorMessage());
    }
}

/**
 * Class responsible for communication with web clients
 */
export default class WebProxyServer {
    /**
     * Clients that are connected
     * @type {Array<WebProxyClient>}
     */
    public clients: Array<WebProxyClient> = [];

    /**
     * Spawns new server
     * @param {number} portNumber
     */
    public createServer(portNumber: number): void {
        this.initializeWebsocketServer(portNumber);
    }

    /**
     * @param {WebProxyClient} controlClient
     */
    public webClientLeft(controlClient: WebProxyClient): void {
        controlClient.terminate();

        let index = this.clients.indexOf(controlClient);
        if (index !== -1) {
            this.clients.splice(index, 1);
        } else {
            if (ConfigurationManager.isDebug()) {
                console.error("Could not find client in the client list");
            }
        }

        if (ConfigurationManager.isDebug()) {
            console.log('Web client (' + controlClient.getWebClient().conn.remoteAddress + ') disconnected. Total ' + this.clients.length + ' clients.');
        }
    }

    /**
     * @param {number} portNumber
     */
    protected initializeWebsocketServer(portNumber: number): void {
        SocketIO.listen(portNumber, {pingInterval: 1000, pingTimeout: 5000})
            .on('connection', this.webClientConnectedCallback.bind(this));

        console.log("WebSocket server running at " + portNumber);
    }

    /**
     * Callback triggered when new client is connected
     *
     * @param {SocketIOSocket} webClient
     */
    protected webClientConnectedCallback(webClient: SocketIOSocket): void {
        let controlClient = new WebProxyClient(this, webClient);
        this.clients.push(controlClient);

        if (ConfigurationManager.isDebug()) {
            console.log('New web client (' + webClient.conn.remoteAddress + ') connected. Total ' + this.clients.length + ' clients.');
        }

        controlClient.initialize();
    }
}

let server: WebProxyServer = new WebProxyServer();
server.createServer(5021);
