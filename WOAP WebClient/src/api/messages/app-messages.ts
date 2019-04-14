import ValidatableOSCMessage from "./validatable-osc-message";
import ValidatableOSCMessageWithId from "./validatable-osc-message-with-id";
import {OSCArgumentType} from "../../osc/osc-argument";
import {Utility, UUID} from "../../utility";
import boolToInt = Utility.boolToInt;

export class ApiCoreGetStatusMessage extends ValidatableOSCMessage {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return ApiCoreGetStatusMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/getStatus";
    }
}

export class ApiCoreReadyMessage extends ValidatableOSCMessage {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return ApiCoreReadyMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/ready";
    }
}

export class ApiCoreMessageRejectedMessage extends ValidatableOSCMessage {
    static readonly ResponseCode = {
        InvalidMessage: 1001,
        Unauthorized: 1002,
        ParsingError: 1003
    };

    /**
     * @param {number} responseCode
     * @param {number} messageId
     * @param {string} debugInfo
     * @returns {this}
     */
    public addArguments(responseCode: number, messageId: number, debugInfo: string): this {
        return this.addArgumentSet([
            responseCode,
            messageId,
            debugInfo
        ]);
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer, /* responseCode */
            OSCArgumentType.Integer, /* message id */
            OSCArgumentType.String      /* debug info */
        ];
    }

    /**
     * @returns {number}
     */
    public getResponseCode(): number {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getRejectedMessageId(): number {
        return this.getArgumentByOffset(1);
    }

    /**
     * @returns {number}
     */
    public getMessageDebugInfo(): number {
        return this.getArgumentByOffset(2);
    }

    /**
     * @returns {string}
     */
    public getResponseString(): string {
        switch (this.getResponseCode()) {
            case ApiCoreMessageRejectedMessage.ResponseCode.InvalidMessage:
                return "invalid message";
            case ApiCoreMessageRejectedMessage.ResponseCode.Unauthorized:
                return "unauthorized";
            case ApiCoreMessageRejectedMessage.ResponseCode.ParsingError:
                return "parsing error";
            default:
                return "unknown (" + this.getResponseCode() + ")";
        }
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return ApiCoreMessageRejectedMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/messageRejected";
    }
}

export class ApiCoreErrorMessage extends ValidatableOSCMessage {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return ApiCoreErrorMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/error";
    }
}

export class ApiCoreGetServerInfoMessage extends ValidatableOSCMessageWithId {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return ApiCoreGetServerInfoMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/getServerInfo";
    }
}

export class ApiCoreServerInfoMessage extends ValidatableOSCMessageWithId {
    /**
     * @param {string} deviceVersion
     * @param {string} deviceId
     * @param {string} processId
     * @param {boolean} anonymousLoginEnabled
     * @param {boolean} userAuthenticated
     * @returns {this}
     */
    public addArguments(deviceVersion: string, deviceId: string, processId: string, anonymousLoginEnabled: boolean, userAuthenticated: boolean): this {
        return this.addArgumentSet([
            deviceVersion,
            deviceId,
            processId,
            boolToInt(anonymousLoginEnabled),
            boolToInt(userAuthenticated)
        ]);
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,  // deviceVersion
            OSCArgumentType.String,  // deviceId
            OSCArgumentType.String,  // processId
            OSCArgumentType.Integer, // AnonymousLoginEnabled
            OSCArgumentType.Integer  // User authenticated
        ];
    }

    /**
     * @returns {string}
     */
    public getDeviceVersion(): string {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {UUID}
     */
    public getDeviceId(): UUID {
        return this.getArgumentByOffset(1);
    }

    /**
     * @returns {UUID}
     */
    public getProcessId(): UUID {
        return this.getArgumentByOffset(2);
    }

    /**
     * @returns {boolean}
     */
    public getAnonymousLoginEnabled(): boolean {
        return Utility.intToBool(this.getArgumentByOffset(3));
    }

    /**
     * @returns {boolean}
     */
    public getUserAuthenticated(): boolean {
        return Utility.intToBool(this.getArgumentByOffset(4));
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return ApiCoreServerInfoMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/serverInfo";
    }
}

export class ApiCoreLoginMessage extends ValidatableOSCMessageWithId {
    /**
     * @param {string} password
     * @returns {this}
     */
    public addArguments(password: string): this {
        return this.addArgumentSet([
            password
        ]);
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String  // password
        ];
    }

    /**
     * @returns {string}
     */
    public getPassword(): string {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return ApiCoreLoginMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/login";
    }
}

export class ApiCoreLoginResponseMessage extends ValidatableOSCMessageWithId {
    static readonly ResponseCode = {
        Ok: 1000,
        WrongCredentials: 1001
    };

    /**
     * @param {number} responseCode
     * @param {string} logTag
     * @returns {this}
     */
    public addArguments(responseCode: number, logTag: string = ""): this {
        return this.addArgumentSet([
            responseCode,
            logTag
        ]);
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer, /* responseCode */
            OSCArgumentType.String    /* Log tag */
        ];
    }

    /**
     * @returns {number}
     */
    public getResponseCode(): number {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {string}
     */
    public getResponseString(): string {
        switch (this.getResponseCode()) {
            case ApiCoreLoginResponseMessage.ResponseCode.Ok:
                return "ok";
            case ApiCoreLoginResponseMessage.ResponseCode.WrongCredentials:
                return "wrong credentials entered";
            default:
                return "unknown error (" + this.getResponseCode() + ")";
        }
    }

    /**
     * @returns {string}
     */
    public getLogTag(): string {
        return this.getArgumentByOffset(1);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return ApiCoreLoginResponseMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/loginResponse";
    }
}

export class ApiCoreAuthenticateMessage extends ValidatableOSCMessageWithId {
    /**
     * @param {string} logTag
     * @returns {this}
     */
    public addArguments(logTag: string): this {
        return this.addArgumentSet([
            logTag
        ]);
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,  // logTag
        ];
    }

    /**
     * @returns {string}
     */
    public getLogTag(): string {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return ApiCoreAuthenticateMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/authenticate";
    }
}

export class ApiCoreAuthenticationResponseMessage extends ValidatableOSCMessageWithId {
    static readonly ResponseCode = {
        Ok: 1000,
        LogTagUnknownOrExpired: 1001
    };

    /**
     * @param {number} responseCode
     * @returns {this}
     */
    public addArguments(responseCode: number): this {
        return this.addArgumentSet([
            responseCode
        ]);
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer, /* responseCode */
        ];
    }

    /**
     * @returns {number}
     */
    public getResponseCode(): number {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {string}
     */
    public getResponseString(): string {
        switch (this.getResponseCode()) {
            case ApiCoreAuthenticationResponseMessage.ResponseCode.Ok:
                return "ok";
            case ApiCoreAuthenticationResponseMessage.ResponseCode.LogTagUnknownOrExpired:
                return "log tag expired";
            default:
                return "unknown (" + this.getResponseCode() + ")";
        }
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return ApiCoreAuthenticationResponseMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/authenticationResponse";
    }
}
