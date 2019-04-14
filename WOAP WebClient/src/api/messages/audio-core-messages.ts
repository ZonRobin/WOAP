import {UUID} from "../../utility";
import ValidatableOSCMessage from "./validatable-osc-message";
import {OSCArgumentType} from "../../osc/osc-argument";
import ValidatableOSCMessageWithId from "./validatable-osc-message-with-id";

/**
 *  [UUID, name, color, active]
 */
export type AvailableMixerNodeInfo = [UUID, string, string, boolean];

export class AudioCoreAddTrackMessage extends ValidatableOSCMessage {

    /**
     * @param {number} trackType
     * @param {number} position
     * @returns {this}
     */
    public addArguments(trackType: number, position: number): this {
        return this.addArgumentSet([trackType, position]);
    }

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer,
            OSCArgumentType.Integer
        ];
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreAddTrackMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/addTrack";
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
    public getTrackPosition(): number {
        return this.getArgumentByOffset(1);
    }
}

export class AudioCoreTrackAddedMessage extends ValidatableOSCMessage {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,
            OSCArgumentType.Integer
        ];
    }

    /**
     * @param {UUID} trackId
     * @param {number} trackPosition
     * @returns {this}
     */
    public addArguments(trackId: UUID, trackPosition: number): this {
        return this.addArgumentSet([trackId]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreTrackAddedMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/trackAdded";
    }

    /**
     * @returns {UUID}
     */
    public getTrackId(): UUID {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getTrackPosition(): number {
        return this.getArgumentByOffset(1);
    }
}

export class AudioCoreRemoveTrackMessage extends ValidatableOSCMessage {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String
        ];
    }

    /**
     *
     * @param {UUID} trackId
     * @returns {this}
     */
    public addArguments(trackId: UUID): this {
        return this.addArgumentSet([trackId]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreRemoveTrackMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/removeTrack";
    }

    /**
     * @returns {UUID}
     */
    public getTrackId(): UUID {
        return this.getArgumentByOffset(0);
    }
}

export class AudioCoreTrackRemovedMessage extends ValidatableOSCMessage {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String
        ];
    }

    /**
     * @param {UUID} trackId
     * @returns {this}
     */
    public addArguments(trackId: UUID): this {
        return this.addArgumentSet([trackId]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreTrackRemovedMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/trackRemoved";
    }

    /**
     * @returns {UUID}
     */
    public getTrackId(): UUID {
        return this.getArgumentByOffset(0);
    }
}


export class AudioCoreMoveTrackMessage extends ValidatableOSCMessage {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,
            OSCArgumentType.Integer
        ];
    }

    /**
     * @param {UUID} trackId
     * @param {number} newPosition
     * @returns {this}
     */
    public addArguments(trackId: UUID, newPosition: number): this {
        return this.addArgumentSet([trackId, newPosition]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreMoveTrackMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/moveTrack";
    }

    /**
     * @returns {UUID}
     */
    public getTrackId(): UUID {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getNewTrackPosition(): number {
        return this.getArgumentByOffset(1);
    }
}


export class AudioCoreTrackMovedMessage extends ValidatableOSCMessage {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,
            OSCArgumentType.Integer,
            OSCArgumentType.Integer
        ];
    }

    /**
     * @param {UUID} trackId
     * @param {number} oldPosition
     * @param {number} newPosition
     * @returns {this}
     */
    public addArguments(trackId: UUID, oldPosition: number, newPosition: number): this {
        return this.addArgumentSet([trackId, oldPosition, newPosition]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreTrackMovedMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/trackMoved";
    }

    /**
     * @returns {UUID}
     */
    public getTrackId(): UUID {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getOldTrackPosition(): number {
        return this.getArgumentByOffset(1);
    }

    /**
     * @returns {number}
     */
    public getNewTrackPosition(): number {
        return this.getArgumentByOffset(2);
    }
}

export class AudioCoreConnectNodesMessage extends ValidatableOSCMessage {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,
            OSCArgumentType.String
        ];
    }

    /**
     * @param {UUID} sourceNodeId
     * @param {UUID} receiverNodeId
     * @returns {this}
     */
    public addArguments(sourceNodeId: UUID, receiverNodeId: UUID): this {
        return this.addArgumentSet([sourceNodeId, receiverNodeId]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreConnectNodesMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/connectNodes";
    }

    /**
     * @returns {UUID}
     */
    public getSourceNodeId(): UUID {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {UUID}
     */
    public getReceiverNodeId(): UUID {
        return this.getArgumentByOffset(1);
    }
}

export class AudioCoreRemoveNodesConnectionMessage extends ValidatableOSCMessage {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,
            OSCArgumentType.String
        ];
    }

    /**
     * @param {UUID} sourceNodeId
     * @param {UUID} receiverNodeId
     * @returns {this}
     */
    public addArguments(sourceNodeId: UUID, receiverNodeId: UUID): this {
        return this.addArgumentSet([sourceNodeId, receiverNodeId]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreRemoveNodesConnectionMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/removeNodesConnection";
    }

    /**
     * @returns {UUID}
     */
    public getSourceNodeId(): UUID {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {UUID}
     */
    public getReceiverNodeId(): UUID {
        return this.getArgumentByOffset(1);
    }
}

export class AudioCoreConnectionsChangedMessage extends ValidatableOSCMessage {

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreConnectionsChangedMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/connectionsChanged";
    }
}

export class AudioCoreGetTracksListMessage extends ValidatableOSCMessageWithId {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreGetTracksListMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/getTracksList";
    }
}

export class AudioCoreTracksListMessage extends ValidatableOSCMessageWithId {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String
        ];
    }

    /**
     *
     * @param {UUID[]} trackIds
     * @returns {this}
     */
    public addArguments(trackIds: UUID[]): this {
        return this.addArgumentSet([JSON.stringify(trackIds)]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreTracksListMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/tracksList";
    }

    /**
     * @returns {UUID[]}
     */
    public getTrackIds(): UUID[] {
        return JSON.parse(this.getArgumentByOffset(0));
    }
}

export class AudioCoreGetInputProxiesListMessage extends ValidatableOSCMessageWithId {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreGetInputProxiesListMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/getInputProxiesList";
    }
}

export class AudioCoreInputProxiesListMessage extends ValidatableOSCMessageWithId {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String
        ];
    }

    /**
     *
     * @param {UUID[]} proxyIds
     * @returns {this}
     */
    public addArguments(proxyIds: UUID[]): this {
        return this.addArgumentSet([JSON.stringify(proxyIds)]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreInputProxiesListMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/inputProxiesList";
    }

    /**
     * @returns {UUID[]}
     */
    public getProxyIds(): UUID[] {
        return JSON.parse(this.getArgumentByOffset(0));
    }
}

export class AudioCoreInputProxyAddedMessage extends ValidatableOSCMessage {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,
            OSCArgumentType.String
        ];
    }

    /**
     * @param {UUID} proxyId
     * @param {string} proxyName
     * @returns {this}
     */
    public addArguments(proxyId: UUID, proxyName: string): this {
        return this.addArgumentSet([proxyId, proxyName]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreInputProxyAddedMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/inputProxyAdded";
    }

    /**
     * @returns {UUID}
     */
    public getProxyId(): UUID {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {string}
     */
    public getProxyName(): string {
        return this.getArgumentByOffset(1);
    }
}

export class AudioCoreInputProxyRemovedMessage extends ValidatableOSCMessage {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String
        ];
    }

    /**
     * @param {UUID} proxyId
     * @returns {this}
     */
    public addArguments(proxyId: UUID): this {
        return this.addArgumentSet([proxyId]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreInputProxyRemovedMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/inputProxyRemoved";
    }

    /**
     * @returns {UUID}
     */
    public getProxyId(): UUID {
        return this.getArgumentByOffset(0);
    }
}

export class AudioCoreGetOutputProxiesListMessage extends ValidatableOSCMessageWithId {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreGetOutputProxiesListMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/getOutputProxiesList";
    }
}

export class AudioCoreOutputProxiesListMessage extends ValidatableOSCMessageWithId {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String
        ];
    }

    /**
     *
     * @param {UUID[]} proxyIds
     * @returns {this}
     */
    public addArguments(proxyIds: UUID[]): this {
        return this.addArgumentSet([JSON.stringify(proxyIds)]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreOutputProxiesListMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/outputProxiesList";
    }

    /**
     * @returns {UUID[]}
     */
    public getProxyIds(): UUID[] {
        return JSON.parse(this.getArgumentByOffset(0));
    }
}

export class AudioCoreOutputProxyAddedMessage extends ValidatableOSCMessage {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String,
            OSCArgumentType.String
        ];
    }

    /**
     * @param {UUID} proxyId
     * @param {string} proxyName
     * @returns {this}
     */
    public addArguments(proxyId: UUID, proxyName: string): this {
        return this.addArgumentSet([proxyId, proxyName]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreOutputProxyAddedMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/outputProxyAdded";
    }

    /**
     * @returns {UUID}
     */
    public getProxyId(): UUID {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {string}
     */
    public getProxyName(): string {
        return this.getArgumentByOffset(1);
    }
}

export class AudioCoreOutputProxyRemovedMessage extends ValidatableOSCMessage {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String
        ];
    }

    /**
     * @param {UUID} proxyId
     * @returns {this}
     */
    public addArguments(proxyId: UUID): this {
        return this.addArgumentSet([proxyId]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreOutputProxyRemovedMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/outputProxyRemoved";
    }

    /**
     * @returns {UUID}
     */
    public getProxyId(): UUID {
        return this.getArgumentByOffset(0);
    }
}

export class AudioCoreGetAvailableInputSourcesListMessage extends ValidatableOSCMessageWithId {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String
        ];
    }

    /**
     * @param {UUID} receiverNodeId
     * @returns {this}
     */
    public addArguments(receiverNodeId: UUID): this {
        return this.addArgumentSet([receiverNodeId]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreGetAvailableInputSourcesListMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/getAvailableInputSourcesList";
    }

    /**
     * @returns {UUID}
     */
    public getReceiverNodeId(): UUID {
        return this.getArgumentByOffset(0);
    }
}

export class AudioCoreAvailableInputSourcesListMessage extends ValidatableOSCMessageWithId {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String
        ];
    }

    /**
     * @param {AvailableMixerNodeInfo[]} sources
     * @returns {this}
     */
    public addArguments(sources: AvailableMixerNodeInfo[]): this {
        return this.addArgumentSet([JSON.stringify(sources)]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreAvailableInputSourcesListMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/availableInputSourcesList";
    }

    /**
     * @returns {AvailableMixerNodeInfo[]}
     */
    public getSources(): AvailableMixerNodeInfo[] {
        return JSON.parse(this.getArgumentByOffset(0));
    }
}

export class AudioCoreGetAvailableOutputReceiversListMessage extends ValidatableOSCMessageWithId {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String
        ];
    }

    /**
     * @param {UUID} sourceNodeId
     * @returns {this}
     */
    public addArguments(sourceNodeId: UUID): this {
        return this.addArgumentSet([sourceNodeId]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreGetAvailableOutputReceiversListMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/getAvailableOutputReceiversList";
    }

    /**
     * @returns {UUID}
     */
    public getSourceNodeId(): UUID {
        return this.getArgumentByOffset(0);
    }
}

export class AudioCoreAvailableOutputReceiversListMessage extends ValidatableOSCMessageWithId {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.String
        ];
    }

    /**
     *
     * @param {AvailableMixerNodeInfo[]} receivers
     * @returns {this}
     */
    public addArguments(receivers: AvailableMixerNodeInfo[]): this {
        return this.addArgumentSet([JSON.stringify(receivers)]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreAvailableOutputReceiversListMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/availableOutputReceiversList";
    }

    /**
     * @returns {AvailableMixerNodeInfo[]}
     */
    public getReceivers(): AvailableMixerNodeInfo[] {
        return JSON.parse(this.getArgumentByOffset(0));
    }
}

export class AudioCoreGetGlobalAudioLatencyMessage extends ValidatableOSCMessageWithId {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreGetGlobalAudioLatencyMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/getGlobalAudioLatency";
    }
}

export class AudioCoreGlobalAudioLatencyMessage extends ValidatableOSCMessageWithId {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer,
            OSCArgumentType.Float
        ];
    }

    /**
     * @param {number} latencySamples
     * @param {number} latencyMs
     * @returns {this}
     */
    public addArguments(latencySamples: number, latencyMs: number): this {
        return this.addArgumentSet([latencySamples, latencyMs]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreGlobalAudioLatencyMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/globalAudioLatency";
    }

    /**
     * @returns {number}
     */
    public getLatencySamples(): number {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getLatencyMs(): number {
        return this.getArgumentByOffset(1);
    }
}

export class AudioCoreGlobalAudioLatencyChangedMessage extends ValidatableOSCMessage {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer,
            OSCArgumentType.Float
        ];
    }

    /**
     * @param {number} latencySamples
     * @param {number} latencyMs
     * @returns {this}
     */
    public addArguments(latencySamples: number, latencyMs: number): this {
        return this.addArgumentSet([latencySamples, latencyMs]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreGlobalAudioLatencyChangedMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/globalAudioLatencyChanged";
    }

    /**
     * @returns {number}
     */
    public getLatencySamples(): number {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getLatencyMs(): number {
        return this.getArgumentByOffset(1);
    }
}

export class AudioCoreSignalLevelsMessage extends ValidatableOSCMessage {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer,
            OSCArgumentType.Blob,
            OSCArgumentType.Blob,
            OSCArgumentType.Blob
        ];
    }

    /**
     * @param {number} numTracks
     * @param {Buffer} tracksSignalLevels
     * @param {Buffer} soloBusSignalLevels
     * @param {Buffer} trackClipData
     * @returns {this}
     */
    public addArguments(numTracks: number, tracksSignalLevels: Buffer, soloBusSignalLevels: Buffer, trackClipData: Buffer): this {
        return this.addArgumentSet([numTracks, tracksSignalLevels, soloBusSignalLevels, trackClipData]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreSignalLevelsMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/signalLevels";
    }

    /**
     * @returns {number}
     */
    public getNumTracks(): number {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {DataView}
     */
    public getTracksSignalLevels(): DataView {
        let data = this.getArgumentByOffset(1);
        return new DataView(data.buffer, data.byteOffset, data.length);
    }

    /**
     * @returns {DataView}
     */
    public getSoloBusSignalLevels(): DataView {
        let data = this.getArgumentByOffset(2);
        return new DataView(data.buffer, data.byteOffset, data.length);
    }

    /**
     * @returns {DataView}
     */
    public getTrackClipData(): DataView {
        let data = this.getArgumentByOffset(3);
        return new DataView(data.buffer, data.byteOffset, data.length);
    }
}

export class AudioCoreAddListenerMessage extends ValidatableOSCMessage {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreAddListenerMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/addListener";
    }
}

export class AudioCoreRemoveListenerMessage extends ValidatableOSCMessage {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreRemoveListenerMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/removeListener";
    }
}

export class AudioCoreAddSignalLevelListenerMessage extends ValidatableOSCMessage {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer
        ];
    }

    /**
     * @param {number} refreshIntervalHz
     * @returns {this}
     */
    public addArguments(refreshIntervalHz: number): this {
        return this.addArgumentSet([refreshIntervalHz]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreAddSignalLevelListenerMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/addSignalLevelListener";
    }

    /**
     * @returns {number}
     */
    public getRefreshIntervalMs(): number {
        return this.getArgumentByOffset(0);
    }
}

export class AudioCoreRemoveSignalLevelListenerMessage extends ValidatableOSCMessage {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreRemoveSignalLevelListenerMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/removeSignalLevelListener";
    }
}

export class AudioCoreAudioDataMessage extends ValidatableOSCMessage {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer,
            OSCArgumentType.Integer,
            OSCArgumentType.Blob
        ];
    }

    /**
     * @param {number} numSamples
     * @param {number} numChannels
     * @param {Buffer} audioSamples
     * @returns {this}
     */
    public addArguments(numSamples: number, numChannels: number, audioSamples: Buffer): this {
        return this.addArgumentSet([numSamples, numChannels, audioSamples]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return AudioCoreAudioDataMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/audioCore/audioData";
    }

    /**
     * @returns {number}
     */
    public getNumSamples(): number {
        return this.getArgumentByOffset(0);
    }

    /**
     * @returns {number}
     */
    public getNumChannels(): number {
        return this.getArgumentByOffset(1);
    }

    /**
     * @returns {DataView}
     */
    public getAudioSamples(): DataView {
        let data = this.getArgumentByOffset(2);
        return new DataView(data.buffer, data.byteOffset, data.length);
    }
}