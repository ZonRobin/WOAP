import ValidatableOSCMessage from "./validatable-osc-message";
import OSCMessage from "../../osc/osc-message";
import {OSCArgumentType} from "../../osc/osc-argument";
import ValidatableOSCMessageWithId from "./validatable-osc-message-with-id";

/**
 *  [uid, name, manufacturerName]
 */
export type PluginTypeInfo = [number, string, string];

export class PluginManagerGetPluginTypesMessage extends ValidatableOSCMessageWithId {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginManagerGetPluginTypesMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/pluginManager/getPluginTypes";
    }
}

export class PluginManagerPluginTypesMessage extends ValidatableOSCMessageWithId {

    /**
     * @param {OSCMessage} message
     */
    public constructor(message: OSCMessage = null) {
        super(message, false);
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String      // pluginTypes JSON
        ];
    }

    /**
     * @param {PluginTypeInfo[]} pluginTypes
     * @returns {this}
     */
    public addArguments(pluginTypes: PluginTypeInfo[]): this {
        return this.addArgumentSet([
            JSON.stringify(pluginTypes)
        ]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginManagerPluginTypesMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/pluginManager/pluginTypes";
    }

    /**
     * @returns {PluginTypeInfo[]}
     */
    public getPluginTypes(): PluginTypeInfo[] {
        return JSON.parse(this.getArgumentByOffset(0));
    }
}

export class PluginManagerScanPluginTypesMessage extends ValidatableOSCMessage {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginManagerScanPluginTypesMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/pluginManager/scanPluginTypes";
    }
}

export class PluginManagerScanPluginTypesIfNeededMessage extends ValidatableOSCMessage {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginManagerScanPluginTypesIfNeededMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/pluginManager/scanPluginTypesIfNeeded";
    }
}

export class PluginManagerPluginTypesScanStartedMessage extends ValidatableOSCMessage {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginManagerPluginTypesScanStartedMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/pluginManager/pluginManagerPluginTypesScanStarted";
    }
}

export class PluginManagerPluginTypesScannedMessage extends ValidatableOSCMessage {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginManagerPluginTypesScannedMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/pluginManager/pluginManagerPluginTypesScanned";
    }
}

export class PluginManagerPluginTypeAddedMessage extends ValidatableOSCMessageWithId {

    /**
     * @param {OSCMessage} message
     */
    public constructor(message: OSCMessage = null) {
        super(message, false);
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer,     // plugin type uid
            OSCArgumentType.String,      // plugin type name
            OSCArgumentType.String,      // plugin type manufacturer name
        ];
    }

    /**
     * @param {number} pluginTypeUid
     * @param {string} pluginTypeName
     * @param {string} pluginTypeManufacturerName
     * @returns {this}
     */
    public addArguments(pluginTypeUid: number, pluginTypeName: string, pluginTypeManufacturerName: string): this {
        return this.addArgumentSet([
            pluginTypeUid,
            pluginTypeName,
            pluginTypeManufacturerName
        ]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginManagerPluginTypeAddedMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/pluginManager/pluginTypeAdded";
    }

    /**
     * @returns {number}
     */
    public getPluginTypeUid(): number {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {string}
     */
    public getPluginTypeName(): string {
        return this.getArgumentByOffset(1);
    }

    /**
     * @returns {string}
     */
    public getPluginTypeManufacturerName(): string {
        return this.getArgumentByOffset(2);
    }
}

export class PluginManagerAddListenerMessage extends ValidatableOSCMessage {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginManagerAddListenerMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/pluginManager/addListener";
    }
}

export class PluginManagerRemoveListenerMessage extends ValidatableOSCMessage {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginManagerRemoveListenerMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/pluginManager/removeListener";
    }
}