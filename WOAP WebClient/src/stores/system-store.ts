import {ApiCoreServerInfoMessage} from "../api/messages/app-messages";
import {Utility} from "../utility";
import Store from "./store";

export interface SystemStoreListener {
    connectionStatusChanged(connected: boolean, hostReady: boolean): void;

    serverInfoChanged(info: ServerInfo): void;

    authenticatedStatusChanged(authenticated: boolean): void;

    logTagChanged(logTag: string): void;
}

export class ServerInfo {
    protected deviceId: string;
    protected deviceVersion: string;
    protected processId: string;
    protected anonymousLoginEnabled: boolean;

    /**
     * @returns {string}
     */
    public getDeviceId(): string {
        return this.deviceId;
    }

    /**
     * @returns {string}
     */
    public getDeviceVersion(): string {
        return this.deviceVersion;
    }

    /**
     * @returns {string}
     */
    public getProcessId(): string {
        return this.processId;
    }

    /**
     * @returns {boolean}
     */
    public isAnonymousLoginEnabled(): boolean {
        return this.anonymousLoginEnabled;
    }

    /**
     * @param {ApiCoreServerInfoMessage} msg
     * @returns {ServerInfo}
     */
    public static fromOSCMessage(msg: ApiCoreServerInfoMessage): ServerInfo {
        let info = new ServerInfo();
        info.deviceId = msg.getDeviceId();
        info.deviceVersion = msg.getDeviceVersion();
        info.processId = msg.getProcessId();
        info.anonymousLoginEnabled = msg.getAnonymousLoginEnabled();
        return info;
    }

    /**
     * @param {ServerInfo} other
     * @returns {boolean}
     */
    public equalsTo(other: ServerInfo): boolean {
        return other !== null &&
            this.processId === other.processId &&
            this.anonymousLoginEnabled === other.anonymousLoginEnabled &&
            this.deviceVersion === other.deviceVersion &&
            this.deviceId === other.deviceId;
    }
}

export default class SystemStore extends Store<SystemStoreListener> {
    protected connected: boolean = false;
    protected hostReady: boolean = false;
    protected serverInfo: ServerInfo = null;
    protected authenticated: boolean = false;
    protected logTag: string;

    public constructor() {
        super();
        this.logTag = Utility.getCookie("logTag");
    }

    public reloadStore(): void {
    }

    public isConnected(): boolean {
        return this.connected;
    }

    public isHostReady(): boolean {
        return this.hostReady;
    }

    public getServerInfo(): ServerInfo {
        return this.serverInfo;
    }

    public isAuthenticated(): boolean {
        return this.authenticated;
    }

    public getLogTag(): string {
        return this.logTag;
    }

    public setConnectionStatus(connected: boolean, hostReady: boolean): this {
        let previousConnected = this.connected;
        let previousHostReady = this.hostReady;
        this.connected = connected;
        this.hostReady = hostReady;
        if (this.connected !== previousConnected || this.hostReady !== previousHostReady) {
            this.listeners.forEach((listener) => listener.connectionStatusChanged(this.connected, this.hostReady));
        }

        return this;
    }

    public setServerInfo(newServerInfo: ServerInfo): this {
        if (!newServerInfo.equalsTo(this.serverInfo)) {
            this.serverInfo = newServerInfo;
            this.listeners.forEach((listener) => listener.serverInfoChanged(this.serverInfo));
        }

        return this;
    }

    public setAuthenticated(newAuthenticated: boolean): this {
        if (this.authenticated !== newAuthenticated) {
            this.authenticated = newAuthenticated;
            this.listeners.forEach((listener) => listener.authenticatedStatusChanged(this.authenticated));
        }

        return this;
    }

    public setLogTag(newLogTag: string): this {
        if (this.logTag !== newLogTag) {
            this.logTag = newLogTag;
            Utility.setCookie("logTag", this.logTag);
            this.listeners.forEach((listener) => listener.logTagChanged(this.logTag));
        }

        return this;
    }
}