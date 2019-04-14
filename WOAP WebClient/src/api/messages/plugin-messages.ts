import ValidatableOSCMessage from "./validatable-osc-message";
import {UUID, Utility} from "../../utility";
import OSCMessage from "../../osc/osc-message";
import {OSCArgumentType} from "../../osc/osc-argument";
import ValidatableOSCMessageWithId from "./validatable-osc-message-with-id";
import intToBool = Utility.intToBool;
import boolToInt = Utility.boolToInt;

/**
 *  [index, ref, value, valueText]
 */
export type PluginParameterInfo = [number, string, number, string];

/**
 * [value, valueText]
 */
export type PluginParameterValue = [number, string];

export class PluginGetDetailMessage extends ValidatableOSCMessageWithId {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginGetDetailMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/getDetail";
    }
}

export class PluginDetailMessage extends ValidatableOSCMessageWithId {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,     // name
            OSCArgumentType.String,     // ref
            OSCArgumentType.Integer,    // typeId
            OSCArgumentType.Integer,    // bypassed
            OSCArgumentType.Integer     // active
        ];
    }

    /**
     * @param {string} name
     * @param {string} ref
     * @param {number} typeId
     * @param {boolean} bypassed
     * @param {boolean} active
     * @returns {this}
     */
    public addArguments(name: string,
                        ref: string,
                        typeId: number,
                        bypassed: boolean,
                        active: boolean): this {
        return this.addArgumentSet([
            name,
            ref,
            typeId,
            boolToInt(bypassed),
            boolToInt(active),
        ]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginDetailMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/detail";
    }

    /**
     * @returns {string}
     */
    public getName(): string {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {string}
     */
    public getRef(): string {
        return this.getArgumentByOffset(1);
    }

    /**
     * @returns {number}
     */
    public getTypeId(): number{
        return this.getArgumentByOffset(2);
    }

    /**
     * @returns {boolean}
     */
    public getBypassedState(): boolean {
        return intToBool(this.getArgumentByOffset(3));
    }

    /**
     * @returns {boolean}
     */
    public getActiveState(): boolean {
        return intToBool(this.getArgumentByOffset(4));
    }
}

export class PluginGetParameterValues extends ValidatableOSCMessageWithId {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginGetParameterValues.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/getParameterValues";
    }
}

export class PluginParameterValues extends ValidatableOSCMessageWithId {
    protected pluginId: UUID;

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String /* JSON array of values */
        ];
    }

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @param {number[]} values
     * @returns {this}
     */
    public addArguments(values: number[]): this {
        return this.addArgumentSet([JSON.stringify(values)]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginParameterValues.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/parameterValues";
    }

    /**
     * @returns {PluginParameterValue[]}
     */
    public getParameterValues(): PluginParameterValue[] {
        return JSON.parse(this.getArgumentByOffset(0));
    }
}

export class PluginGetParameters extends ValidatableOSCMessageWithId {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginGetParameters.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/getParameters";
    }
}

export class PluginParameters extends ValidatableOSCMessageWithId {
    protected pluginId: UUID;

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String /* JSON array of parameter info */
        ];
    }

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @param {PluginParameterInfo[]} parametersInfo
     * @returns {this}
     */
    public addArguments(parametersInfo: PluginParameterInfo[]): this {
        return this.addArgumentSet([JSON.stringify(parametersInfo)]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginParameters.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/parameters";
    }

    /**
     * @returns {PluginParameterInfo[]}
     */
    public getParameters(): PluginParameterInfo[] {
        return JSON.parse(this.getArgumentByOffset(0));
    }
}

export class PluginSetNameMessage extends ValidatableOSCMessage {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String
        ];
    }

    /**
     * @param {string} newName
     * @returns {this}
     */
    public addArguments(newName: string): this {
        return this.addArgumentSet([newName]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginSetNameMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/setName";
    }

    /**
     * @returns {string}
     */
    public getNewName(): string {
        return this.getArgumentByOffset(0);
    }
}

export class PluginNameChangedMessage extends ValidatableOSCMessage {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String
        ];
    }

    /**
     * @param {string} newName
     * @returns {this}
     */
    public addArguments(newName: string): this {
        return this.addArgumentSet([newName]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginNameChangedMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/nameChanged";
    }

    /**
     * @returns {string}
     */
    public getNewName(): string {
        return this.getArgumentByOffset(0);
    }
}

export class PluginSetActiveMessage extends ValidatableOSCMessage {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer
        ];
    }

    /**
     * @param {boolean} newState
     * @returns {this}
     */
    public addArguments(newState: boolean): this {
        return this.addArgumentSet([boolToInt(newState)]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginSetActiveMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/setActive";
    }

    /**
     * @returns {boolean}
     */
    public getNewState(): boolean {
        return intToBool(this.getArgumentByOffset(0));
    }
}

export class PluginActiveChangedMessage extends ValidatableOSCMessage {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer
        ];
    }

    /**
     * @param {boolean} newState
     * @returns {this}
     */
    public addArguments(newState: boolean): this {
        return this.addArgumentSet([boolToInt(newState)]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginActiveChangedMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/activeChanged";
    }

    /**
     * @returns {boolean}
     */
    public getNewState(): boolean {
        return intToBool(this.getArgumentByOffset(0));
    }
}

export class PluginSetBypassMessage extends ValidatableOSCMessage {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer
        ];
    }

    /**
     * @param {boolean} newState
     * @returns {this}
     */
    public addArguments(newState: boolean): this {
        return this.addArgumentSet([boolToInt(newState)]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginSetBypassMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/setBypass";
    }

    /**
     * @returns {boolean}
     */
    public getNewState(): boolean {
        return intToBool(this.getArgumentByOffset(0));
    }
}

export class PluginBypassChangedMessage extends ValidatableOSCMessage {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer
        ];
    }

    /**
     * @param {boolean} newState
     * @returns {this}
     */
    public addArguments(newState: boolean): this {
        return this.addArgumentSet([boolToInt(newState)]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginBypassChangedMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/bypassChanged";
    }

    /**
     * @returns {boolean}
     */
    public getNewState(): boolean {
        return intToBool(this.getArgumentByOffset(0));
    }
}

export class PluginSetParameterValueMessage extends ValidatableOSCMessage {
    protected pluginId: UUID;

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer, /* parameter index */
            OSCArgumentType.Float /* new value */
        ];
    }

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @param {number} parameterIndex
     * @param {number} newValue
     * @returns {this}
     */
    public addArguments(parameterIndex: number, newValue: number): this {
        return this.addArgumentSet([parameterIndex, newValue]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginSetParameterValueMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/setParameterValue";
    }

    /**
     * @returns {number}
     */
    public getParameterIndex(): number {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getNewValue(): number {
        return this.getArgumentByOffset(1);
    }
}

export class PluginParameterValueChangedMessage extends ValidatableOSCMessage {
    protected pluginId: UUID;

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer,    /* Parameter index */
            OSCArgumentType.Float,      /* New value */
            OSCArgumentType.String      /* New value text */
        ];
    }

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @param {number} parameterIndex
     * @param {number} newValue
     * @param {string} newValueText
     * @returns {PluginParameterValueChangedMessage}
     */
    public addArguments(parameterIndex: number, newValue: number, newValueText: string): this {
        return this.addArgumentSet([parameterIndex, newValue, newValueText]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginParameterValueChangedMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/parameterValueChanged";
    }

    /**
     * @returns {number}
     */
    public getParameterIndex(): number {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getNewValue(): number {
        return this.getArgumentByOffset(1);
    }

    /**
     * @returns {string}
     */
    public getNewValueText(): string {
        return this.getArgumentByOffset(2);
    }
}

export class PluginAddListenerMessage extends ValidatableOSCMessage {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginAddListenerMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/addListener";
    }
}

export class PluginRemoveListenerMessage extends ValidatableOSCMessage {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginRemoveListenerMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/removeListener";
    }
}

export class PluginAddParameterListenerMessage extends ValidatableOSCMessage {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginAddParameterListenerMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/addParameterListener";
    }
}

export class PluginRemoveParameterListenerMessage extends ValidatableOSCMessage {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginRemoveParameterListenerMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/removeParameterListener";
    }
}

export class PluginAddBypassListenerMessage extends ValidatableOSCMessage {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginAddBypassListenerMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/addBypassListener";
    }
}

export class PluginRemoveBypassListenerMessage extends ValidatableOSCMessage {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginRemoveBypassListenerMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/removeBypassListener";
    }
}

export class PluginAddActiveListenerMessage extends ValidatableOSCMessage {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginAddActiveListenerMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/addActiveListener";
    }
}

export class PluginRemoveActiveListenerMessage extends ValidatableOSCMessage {
    protected pluginId: UUID;

    /**
     * @param {UUID} pluginId
     * @param {OSCMessage} message
     */
    public constructor(pluginId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.pluginId = pluginId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return PluginRemoveActiveListenerMessage.getAddress(this.pluginId);
    }

    /**
     * @param {UUID} pluginId
     * @returns {string}
     */
    public static getAddress(pluginId: UUID): string {
        return "/plugin/" + pluginId + "/removeActiveListener";
    }
}