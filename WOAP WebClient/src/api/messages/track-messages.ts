import ValidatableOSCMessage from "./validatable-osc-message";
import OSCMessage from "../../osc/osc-message";
import {OSCArgumentType} from "../../osc/osc-argument";
import {UUID, Utility} from "../../utility";
import ValidatableOSCMessageWithId from "./validatable-osc-message-with-id";
import boolToInt = Utility.boolToInt;
import intToBool = Utility.intToBool;
import Color from "../../helpers/color/color";

export class TrackGetDetailMessage extends ValidatableOSCMessageWithId {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackGetDetailMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/getDetail";
    }
}

export class TrackDetailMessage extends ValidatableOSCMessageWithId {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer,    // track type
            OSCArgumentType.Float,      // gain
            OSCArgumentType.Float,      // pan
            OSCArgumentType.Integer,    // muted
            OSCArgumentType.Integer,    // soloed
            OSCArgumentType.Integer,    // record armed
            OSCArgumentType.String,     // name
            OSCArgumentType.String,     // color
            OSCArgumentType.String,     // pluginIds JSON
            OSCArgumentType.String      // senIds JSON
        ];
    }

    /**
     * @param {number} trackType
     * @param {number} gainValue
     * @param {number} panValue
     * @param {boolean} muteState
     * @param {boolean} soloState
     * @param {boolean} recordArmState
     * @param {string} name
     * @param {string} color
     * @param {UUID[]} pluginIds
     * @param {UUID[]} sendIds
     * @returns {this}
     */
    public addArguments(trackType: number,
                        gainValue: number,
                        panValue: number,
                        muteState: boolean,
                        soloState: boolean,
                        recordArmState: boolean,
                        name: string,
                        color: string,
                        pluginIds: UUID[],
                        sendIds: UUID[]): this {
        return this.addArgumentSet([
            trackType,
            gainValue,
            panValue,
            boolToInt(muteState),
            boolToInt(soloState),
            boolToInt(recordArmState),
            name,
            color,
            JSON.stringify(pluginIds),
            JSON.stringify(sendIds)
        ]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackDetailMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/detail";
    }

    /**
     * @returns {number}
     */
    public getTrackType(): number {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getGainValue(): number {
        return this.getArgumentByOffset(1);
    }

    /**
     * @returns {number}
     */
    public getPanValue(): number {
        return this.getArgumentByOffset(2);
    }

    /**
     * @returns {boolean}
     */
    public getMuteState(): boolean {
        return intToBool(this.getArgumentByOffset(3));
    }

    /**
     * @returns {boolean}
     */
    public getSoloState(): boolean {
        return intToBool(this.getArgumentByOffset(4));
    }

    /**
     * @returns {boolean}
     */
    public getRecordArmState(): boolean {
        return intToBool(this.getArgumentByOffset(5));
    }

    /**
     * @returns {string}
     */
    public getName(): string {
        return this.getArgumentByOffset(6);
    }

    /**
     * @returns {Color}
     */
    public getColor(): Color {
        return new Color("#" + this.getArgumentByOffset(7));
    }

    /**
     * @returns {UUID[]}
     */
    public getPluginIds(): UUID[] {
        return JSON.parse(this.getArgumentByOffset(8));
    }

    /**
     * @returns {UUID[]}
     */
    public getSendNodeIds(): UUID[] {
        return JSON.parse(this.getArgumentByOffset(9));
    }
}

export class TrackSetGainMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
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
        return TrackSetGainMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/setGain";
    }

    /**
     * @returns {number}
     */
    public getNewLevel(): number {
        return this.getArgumentByOffset(0);
    }
}

export class TrackGainChangedMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
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
        return TrackGainChangedMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/gainChanged";
    }

    /**
     * @returns {number}
     */
    public getNewLevel(): number {
        return this.getArgumentByOffset(0);
    }
}

export class TrackSetPanMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
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
        return TrackSetPanMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/setPan";
    }

    /**
     * @returns {number}
     */
    public getNewLevel(): number {
        return this.getArgumentByOffset(0);
    }
}

export class TrackPanChangedMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
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
        return TrackPanChangedMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/panChanged";
    }

    /**
     * @returns {number}
     */
    public getNewLevel(): number {
        return this.getArgumentByOffset(0);
    }
}

export class TrackSetMuteMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
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
        return TrackSetMuteMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/setMute";
    }

    /**
     * @returns {boolean}
     */
    public getNewState(): boolean {
        return intToBool(this.getArgumentByOffset(0));
    }
}

export class TrackMuteChangedMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
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
        return TrackMuteChangedMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/muteChanged";
    }

    /**
     * @returns {boolean}
     */
    public getNewState(): boolean {
        return intToBool(this.getArgumentByOffset(0));
    }
}

export class TrackSetSoloMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
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
        return TrackSetSoloMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/setSolo";
    }

    /**
     * @returns {boolean}
     */
    public getNewState(): boolean {
        return intToBool(this.getArgumentByOffset(0));
    }
}

export class TrackSoloChangedMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
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
        return TrackSoloChangedMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/soloChanged";
    }

    /**
     * @returns {boolean}
     */
    public getNewState(): boolean {
        return intToBool(this.getArgumentByOffset(0));
    }
}

export class TrackSetRecordArmMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
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
        return TrackSetRecordArmMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/setRecordArm";
    }

    /**
     * @returns {boolean}
     */
    public getNewState(): boolean {
        return intToBool(this.getArgumentByOffset(0));
    }
}

export class TrackRecordArmChangedMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
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
        return TrackRecordArmChangedMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/recordArmChanged";
    }

    /**
     * @returns {boolean}
     */
    public getNewState(): boolean {
        return intToBool(this.getArgumentByOffset(0));
    }
}

export class TrackSetNameMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
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
        return TrackSetNameMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {UUID}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/setName";
    }

    /**
     * @returns {string}
     */
    public getNewName(): string {
        return this.getArgumentByOffset(0);
    }
}

export class TrackNameChangedMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
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
        return TrackNameChangedMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {UUID}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/nameChanged";
    }

    /**
     * @returns {string}
     */
    public getNewName(): string {
        return this.getArgumentByOffset(0);
    }
}

export class TrackSetColorMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
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
     * @param {Color} newColor
     * @returns {this}
     */
    public addArguments(newColor: Color): this {
        return this.addArgumentSet([newColor.getHex()]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackSetColorMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {UUID}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/setColour";
    }

    /**
     * @returns {string}
     */
    public getNewColor(): string {
        return this.getArgumentByOffset(0);
    }
}

export class TrackColorChangedMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
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
     * @param {Color} newColor
     * @returns {this}
     */
    public addArguments(newColor: Color): this {
        return this.addArgumentSet([newColor.getHex()]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackColorChangedMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {UUID}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/colourChanged";
    }

    /**
     * @returns {Color}
     */
    public getNewColor(): Color {
        return new Color("#" + this.getArgumentByOffset(0));
    }
}

export class TrackAddSendNodeMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer,  // audioSourcePosition
            OSCArgumentType.Integer,  // sendPosition
        ];
    }

    /**
     * @param {number} audioSourcePosition
     * @param {number} sendPosition
     * @returns {this}
     */
    public addArguments(audioSourcePosition: number, sendPosition: number): this {
        return this.addArgumentSet([audioSourcePosition, sendPosition]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackAddSendNodeMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {UUID}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/addSendNode";
    }

    /**
     * @returns {number}
     */
    public getAudioSourcePosition(): number {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getSendPosition(): number {
        return this.getArgumentByOffset(1);
    }
}

export class TrackSendAddedMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,  // sendNodeId
            OSCArgumentType.Integer  // sendPosition
        ];
    }

    /**
     * @param {UUID} sendNodeId
     * @param {number} sendPosition
     * @returns {this}
     */
    public addArguments(sendNodeId: UUID, sendPosition: number): this {
        return this.addArgumentSet([sendNodeId, sendPosition]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackSendAddedMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {UUID}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/sendAdded";
    }

    /**
     * @returns {UUID}
     */
    public getSendNodeId(): UUID {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getSendPosition(): number {
        return this.getArgumentByOffset(1);
    }
}

export class TrackRemoveSendNodeMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String  // sendNodeId
        ];
    }

    /**
     * @param {UUID} sendNodeId
     * @returns {this}
     */
    public addArguments(sendNodeId: UUID): this {
        return this.addArgumentSet([sendNodeId]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackRemoveSendNodeMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {UUID}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/removeSendNode";
    }

    /**
     * @returns {UUID}
     */
    public getSendNodeId(): UUID {
        return this.getArgumentByOffset(0);
    }
}

export class TrackSendRemovedMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String  // sendNodeId
        ];
    }

    /**
     * @param {UUID} sendNodeId
     * @returns {this}
     */
    public addArguments(sendNodeId: UUID): this {
        return this.addArgumentSet([sendNodeId]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackSendRemovedMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {UUID}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/sendRemoved";
    }

    /**
     * @returns {UUID}
     */
    public getSendNodeId(): UUID {
        return this.getArgumentByOffset(0);
    }
}

export class TrackMoveSendNodeMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,  // sendNodeId
            OSCArgumentType.Integer  // newPosition
        ];
    }

    /**
     * @param {UUID} sendNodeId
     * @param {number} newPosition
     * @returns {this}
     */
    public addArguments(sendNodeId: UUID, newPosition: number): this {
        return this.addArgumentSet([sendNodeId, newPosition]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackMoveSendNodeMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {UUID}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/moveSendNode";
    }

    /**
     * @returns {UUID}
     */
    public getSendNodeId(): UUID {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getNewPosition(): number {
        return this.getArgumentByOffset(1);
    }
}

export class TrackSendMovedMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,  // sendNodeId
            OSCArgumentType.Integer, // oldPosition
            OSCArgumentType.Integer  // newPosition
        ];
    }

    /**
     * @param {UUID} sendNodeId
     * @param {number} oldPosition
     * @param {number} newPosition
     * @returns {this}
     */
    public addArguments(sendNodeId: UUID, oldPosition: number, newPosition: number): this {
        return this.addArgumentSet([sendNodeId, oldPosition, newPosition]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackSendMovedMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {UUID}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/sendMoved";
    }

    /**
     * @returns {UUID}
     */
    public getSendNodeId(): UUID {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getOldPosition(): number {
        return this.getArgumentByOffset(1);
    }

    /**
     * @returns {number}
     */
    public getNewPosition(): number {
        return this.getArgumentByOffset(2);
    }
}

export class TrackAddPluginMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer,  // pluginTypeId
            OSCArgumentType.Integer   // pluginPosition
        ];
    }

    /**
     * @param {number} pluginTypeId
     * @param {number} pluginPosition
     * @returns {this}
     */
    public addArguments(pluginTypeId: number, pluginPosition: number): this {
        return this.addArgumentSet([pluginTypeId, pluginPosition]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackAddPluginMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {UUID}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/addPlugin";
    }

    /**
     * @returns {number}
     */
    public getPluginTypeId(): number {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getPluginPosition(): number {
        return this.getArgumentByOffset(1);
    }
}

export class TrackPluginAddedMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,  // pluginId
            OSCArgumentType.Integer  // position
        ];
    }

    /**
     * @param {UUID} pluginId
     * @param {number} position
     * @returns {this}
     */
    public addArguments(pluginId: UUID, position: number): this {
        return this.addArgumentSet([pluginId, position]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackPluginAddedMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {UUID}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/pluginAdded";
    }

    /**
     * @returns {UUID}
     */
    public getPluginId(): UUID {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getPluginPosition(): number {
        return this.getArgumentByOffset(1);
    }
}

export class TrackRemovePluginMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String  // pluginId
        ];
    }

    /**
     * @param {UUID} pluginId
     * @returns {this}
     */
    public addArguments(pluginId: UUID): this {
        return this.addArgumentSet([pluginId]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackRemovePluginMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {UUID}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/removePlugin";
    }

    /**
     * @returns {UUID}
     */
    public getPluginId(): UUID {
        return this.getArgumentByOffset(0);
    }
}

export class TrackPluginRemovedMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String  // pluginId
        ];
    }

    /**
     * @param {UUID} pluginId
     * @returns {this}
     */
    public addArguments(pluginId: UUID): this {
        return this.addArgumentSet([pluginId]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackPluginRemovedMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {UUID}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/pluginRemoved";
    }

    /**
     * @returns {UUID}
     */
    public getPluginId(): UUID {
        return this.getArgumentByOffset(0);
    }
}

export class TrackMovePluginMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,  // pluginId
            OSCArgumentType.Integer  // newPosition
        ];
    }

    /**
     * @param {UUID} pluginId
     * @param {number} newPosition
     * @returns {this}
     */
    public addArguments(pluginId: UUID, newPosition: number): this {
        return this.addArgumentSet([pluginId, newPosition]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackMovePluginMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {UUID}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/movePlugin";
    }

    /**
     * @returns {UUID}
     */
    public getPluginId(): UUID {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getNewPosition(): number {
        return this.getArgumentByOffset(1);
    }
}

export class TrackPluginMovedMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,  // pluginId
            OSCArgumentType.Integer, // oldPosition
            OSCArgumentType.Integer  // newPosition
        ];
    }

    /**
     * @param {UUID} pluginId
     * @param {number} oldPosition
     * @param {number} newPosition
     * @returns {this}
     */
    public addArguments(pluginId: UUID, oldPosition: number, newPosition: number): this {
        return this.addArgumentSet([pluginId, oldPosition, newPosition]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackPluginMovedMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {UUID}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/pluginMoved";
    }

    /**
     * @returns {UUID}
     */
    public getPluginId(): UUID {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getOldPosition(): number {
        return this.getArgumentByOffset(1);
    }

    /**
     * @returns {number}
     */
    public getNewPosition(): number {
        return this.getArgumentByOffset(2);
    }
}


export class TrackAddInfoListenerMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackAddInfoListenerMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/addInfoListener";
    }
}

export class TrackRemoveInfoListenerMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackRemoveInfoListenerMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/removeInfoListener";
    }
}


export class TrackAddControlsListenerMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackAddControlsListenerMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/addControlsListener";
    }
}

export class TrackRemoveControlsListenerMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackRemoveControlsListenerMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/removeControlsListener";
    }
}

export class TrackAddPluginsListenerMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackAddPluginsListenerMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/addPluginsListener";
    }
}

export class TrackRemovePluginsListenerMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackRemovePluginsListenerMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/removePluginsListener";
    }
}

export class TrackAddSendsListenerMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackAddSendsListenerMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/addSendsListener";
    }
}

export class TrackRemoveSendsListenerMessage extends ValidatableOSCMessage {
    protected trackId: UUID;

    /**
     * @param {UUID} trackId
     * @param {OSCMessage} message
     */
    public constructor(trackId: UUID, message: OSCMessage = null) {
        super(message, false);
        this.trackId = trackId;
        this.initializeMessage();
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return TrackRemoveSendsListenerMessage.getAddress(this.trackId);
    }

    /**
     * @param {UUID} trackId
     * @returns {string}
     */
    public static getAddress(trackId: UUID): string {
        return "/track/" + trackId + "/removeSendsListener";
    }
}