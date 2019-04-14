import ValidatableOSCMessage from "./validatable-osc-message";
import OSCMessage from "../../osc/osc-message";
import {OSCArgumentType} from "../../osc/osc-argument";
import {UUID} from "../../utility";
import ValidatableOSCMessageWithId from "./validatable-osc-message-with-id";
import Color from "../../helpers/color/color";

export class SendGetDetailMessage extends ValidatableOSCMessageWithId {
    protected sendId: UUID;

    /**
     * @param {UUID} sendId
     * @param {OSCMessage} message
     */
    public constructor(sendId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.sendId = sendId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return SendGetDetailMessage.getAddress(this.sendId);
    }

    /**
     * @param {UUID} sendId
     * @returns {string}
     */
    public static getAddress(sendId: UUID): string {
        return "/send/" + sendId + "/getDetail";
    }
}

export class SendDetailMessage extends ValidatableOSCMessageWithId {
    protected sendId: UUID;

    /**
     * @param {UUID} sendId
     * @param {OSCMessage} message
     */
    public constructor(sendId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.sendId = sendId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Float,      // gain
            OSCArgumentType.Float,      // pan
            OSCArgumentType.String,     // name
            OSCArgumentType.String,     // color
            OSCArgumentType.Integer     // audioSourcePosition
        ];
    }

    /**
     * @param {number} gainValue
     * @param {number} panValue
     * @param {string} name
     * @param {Color} color
     * @param {number} audioSourcePosition
     * @returns {this}
     */
    public addArguments(gainValue: number,
                        panValue: number,
                        name: string,
                        color: Color,
                        audioSourcePosition: number): this {
        return this.addArgumentSet([
            gainValue,
            panValue,
            name,
            color.getHex(),
            audioSourcePosition
        ]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return SendDetailMessage.getAddress(this.sendId);
    }

    /**
     * @param {UUID} sendId
     * @returns {string}
     */
    public static getAddress(sendId: UUID): string {
        return "/send/" + sendId + "/detail";
    }

    /**
     * @returns {number}
     */
    public getGainValue(): number {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getPanValue(): number {
        return this.getArgumentByOffset(1);
    }

    /**
     * @returns {string}
     */
    public getName(): string {
        return this.getArgumentByOffset(2);
    }

    /**
     * @returns {Color}
     */
    public getColor(): Color {
        return new Color("#" + this.getArgumentByOffset(3));
    }

    /**
     * @returns {number}
     */
    public getAudioSourcePosition(): number {
        return this.getArgumentByOffset(4);
    }
}


export class SendSetGainMessage extends ValidatableOSCMessage {
    protected sendId: UUID;

    /**
     * @param {UUID} sendId
     * @param {OSCMessage} message
     */
    public constructor(sendId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.sendId = sendId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Float
        ];
    }

    /**
     * @param {number} newGainLevel
     * @returns {this}
     */
    public addArguments(newGainLevel: number): this {
        return this.addArgumentSet([newGainLevel]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return SendSetGainMessage.getAddress(this.sendId);
    }

    /**
     * @param {UUID} sendId
     * @returns {string}
     */
    public static getAddress(sendId: UUID): string {
        return "/send/" + sendId + "/setGain";
    }

    /**
     * @returns {number}
     */
    public getNewLevel(): number {
        return this.getArgumentByOffset(0);
    }
}

export class SendGainChangedMessage extends ValidatableOSCMessage {
    protected sendId: UUID;

    /**
     * @param {UUID} sendId
     * @param {OSCMessage} message
     */
    public constructor(sendId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.sendId = sendId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Float
        ];
    }

    /**
     * @param {number} newValue
     * @returns {this}
     */
    public addArguments(newValue: number): this {
        return this.addArgumentSet([newValue]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return SendGainChangedMessage.getAddress(this.sendId);
    }

    /**
     * @param {UUID} sendId
     * @returns {string}
     */
    public static getAddress(sendId: UUID): string {
        return "/send/" + sendId + "/gainChanged";
    }

    /**
     * @returns {number}
     */
    public getNewLevel(): number {
        return this.getArgumentByOffset(0);
    }
}

export class SendSetPanMessage extends ValidatableOSCMessage {
    protected sendId: UUID;

    /**
     * @param {UUID} sendId
     * @param {OSCMessage} message
     */
    public constructor(sendId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.sendId = sendId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Float
        ];
    }

    /**
     * @param {number} newPanLevel
     * @returns {this}
     */
    public addArguments(newPanLevel: number): this {
        return this.addArgumentSet([newPanLevel]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return SendSetPanMessage.getAddress(this.sendId);
    }

    /**
     * @param {UUID} sendId
     * @returns {string}
     */
    public static getAddress(sendId: UUID): string {
        return "/send/" + sendId + "/setPan";
    }

    /**
     * @returns {number}
     */
    public getNewLevel(): number {
        return this.getArgumentByOffset(0);
    }
}

export class SendPanChangedMessage extends ValidatableOSCMessage {
    protected sendId: UUID;

    /**
     * @param {UUID} sendId
     * @param {OSCMessage} message
     */
    public constructor(sendId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.sendId = sendId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Float
        ];
    }

    /**
     * @param {number} newValue
     * @returns {this}
     */
    public addArguments(newValue: number): this {
        return this.addArgumentSet([newValue]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return SendPanChangedMessage.getAddress(this.sendId);
    }

    /**
     * @param {UUID} sendId
     * @returns {string}
     */
    public static getAddress(sendId: UUID): string {
        return "/send/" + sendId + "/panChanged";
    }

    /**
     * @returns {number}
     */
    public getNewLevel(): number {
        return this.getArgumentByOffset(0);
    }
}

export class SendSetNameMessage extends ValidatableOSCMessage {
    protected sendId: UUID;

    /**
     * @param {UUID} sendId
     * @param {OSCMessage} message
     */
    public constructor(sendId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.sendId = sendId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,     // newName
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
        return SendSetNameMessage.getAddress(this.sendId);
    }

    /**
     * @param {UUID} sendId
     * @returns {UUID}
     */
    public static getAddress(sendId: UUID): string {
        return "/send/" + sendId + "/setName";
    }

    /**
     * @returns {string}
     */
    public getNewName(): string {
        return this.getArgumentByOffset(0);
    }
}

export class SendNameChangedMessage extends ValidatableOSCMessage {
    protected sendId: UUID;

    /**
     * @param {UUID} sendId
     * @param {OSCMessage} message
     */
    public constructor(sendId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.sendId = sendId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,     // newName
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
        return SendNameChangedMessage.getAddress(this.sendId);
    }

    /**
     * @param {UUID} sendId
     * @returns {UUID}
     */
    public static getAddress(sendId: UUID): string {
        return "/send/" + sendId + "/nameChanged";
    }

    /**
     * @returns {string}
     */
    public getNewName(): string {
        return this.getArgumentByOffset(0);
    }
}

export class SendColorChangedMessage extends ValidatableOSCMessage {
    protected sendId: UUID;

    /**
     * @param {UUID} sendId
     * @param {OSCMessage} message
     */
    public constructor(sendId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.sendId = sendId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,     // newColor
        ];
    }

    /**
     * @param {string} newColor
     * @returns {this}
     */
    public addArguments(newColor: string): this {
        return this.addArgumentSet([newColor]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return SendColorChangedMessage.getAddress(this.sendId);
    }

    /**
     * @param {UUID} sendId
     * @returns {UUID}
     */
    public static getAddress(sendId: UUID): string {
        return "/send/" + sendId + "/colourChanged";
    }

    /**
     * @returns {Color}
     */
    public getNewColor(): Color {
        return new Color("#" + this.getArgumentByOffset(0));
    }
}

export class SendSetAudioSourcePositionMessage extends ValidatableOSCMessage {
    protected sendId: UUID;

    /**
     * @param {UUID} sendId
     * @param {OSCMessage} message
     */
    public constructor(sendId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.sendId = sendId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer,     // newAudioSourcePosition
        ];
    }

    /**
     * @param {number} newAudioSourcePosition
     * @returns {this}
     */
    public addArguments(newAudioSourcePosition: number): this {
        return this.addArgumentSet([newAudioSourcePosition]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return SendSetAudioSourcePositionMessage.getAddress(this.sendId);
    }

    /**
     * @param {UUID} sendId
     * @returns {UUID}
     */
    public static getAddress(sendId: UUID): string {
        return "/send/" + sendId + "/setAudioSourcePosition";
    }

    /**
     * @returns {number}
     */
    public getNewAudioSourcePosition(): number {
        return this.getArgumentByOffset(0);
    }
}

export class SendAudioSourcePositionChangedMessage extends ValidatableOSCMessage {
    protected sendId: UUID;

    /**
     * @param {UUID} sendId
     * @param {OSCMessage} message
     */
    public constructor(sendId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.sendId = sendId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer,     // newAudioSourcePosition
        ];
    }

    /**
     * @param {number} newAudioSourcePosition
     * @returns {this}
     */
    public addArguments(newAudioSourcePosition: number): this {
        return this.addArgumentSet([newAudioSourcePosition]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return SendAudioSourcePositionChangedMessage.getAddress(this.sendId);
    }

    /**
     * @param {UUID} sendId
     * @returns {UUID}
     */
    public static getAddress(sendId: UUID): string {
        return "/send/" + sendId + "/audioSourcePositionChanged";
    }

    /**
     * @returns {number}
     */
    public getNewAudioSourcePosition(): number {
        return this.getArgumentByOffset(0);
    }
}

export class SendAddInfoListenerMessage extends ValidatableOSCMessage {
    protected sendId: UUID;

    /**
     * @param {UUID} sendId
     * @param {OSCMessage} message
     */
    public constructor(sendId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.sendId = sendId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return SendAddInfoListenerMessage.getAddress(this.sendId);
    }

    /**
     * @param {UUID} sendId
     * @returns {string}
     */
    public static getAddress(sendId: UUID): string {
        return "/send/" + sendId + "/addInfoListener";
    }
}

export class SendRemoveInfoListenerMessage extends ValidatableOSCMessage {
    protected sendId: UUID;

    /**
     * @param {UUID} sendId
     * @param {OSCMessage} message
     */
    public constructor(sendId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.sendId = sendId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return SendRemoveInfoListenerMessage.getAddress(this.sendId);
    }

    /**
     * @param {UUID} sendId
     * @returns {string}
     */
    public static getAddress(sendId: UUID): string {
        return "/send/" + sendId + "/removeInfoListener";
    }
}

export class SendAddControlsListenerMessage extends ValidatableOSCMessage {
    protected sendId: UUID;

    /**
     * @param {UUID} sendId
     * @param {OSCMessage} message
     */
    public constructor(sendId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.sendId = sendId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return SendAddControlsListenerMessage.getAddress(this.sendId);
    }

    /**
     * @param {UUID} sendId
     * @returns {string}
     */
    public static getAddress(sendId: UUID): string {
        return "/send/" + sendId + "/addControlsListener";
    }
}

export class SendRemoveControlsListenerMessage extends ValidatableOSCMessage {
    protected sendId: UUID;

    /**
     * @param {UUID} sendId
     * @param {OSCMessage} message
     */
    public constructor(sendId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.sendId = sendId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return SendRemoveControlsListenerMessage.getAddress(this.sendId);
    }

    /**
     * @param {UUID} sendId
     * @returns {string}
     */
    public static getAddress(sendId: UUID): string {
        return "/send/" + sendId + "/removeControlsListener";
    }
}

export class SendAddAudioSourcePositionListenerMessage extends ValidatableOSCMessage {
    protected sendId: UUID;

    /**
     * @param {UUID} sendId
     * @param {OSCMessage} message
     */
    public constructor(sendId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.sendId = sendId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return SendAddAudioSourcePositionListenerMessage.getAddress(this.sendId);
    }

    /**
     * @param {UUID} sendId
     * @returns {string}
     */
    public static getAddress(sendId: UUID): string {
        return "/send/" + sendId + "/addAudioSourcePositionListener";
    }
}

export class SendRemoveAudioSourcePositionListenerMessage extends ValidatableOSCMessage {
    protected sendId: UUID;

    /**
     * @param {UUID} sendId
     * @param {OSCMessage} message
     */
    public constructor(sendId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.sendId = sendId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return SendRemoveAudioSourcePositionListenerMessage.getAddress(this.sendId);
    }

    /**
     * @param {UUID} sendId
     * @returns {string}
     */
    public static getAddress(sendId: UUID): string {
        return "/send/" + sendId + "/removeAudioSourcePositionListener";
    }
}