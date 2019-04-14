import {
    TrackAddControlsListenerMessage,
    TrackAddInfoListenerMessage,
    TrackAddPluginsListenerMessage,
    TrackColorChangedMessage,
    TrackDetailMessage,
    TrackGainChangedMessage,
    TrackGetDetailMessage,
    TrackMuteChangedMessage,
    TrackNameChangedMessage,
    TrackPanChangedMessage,
    TrackPluginAddedMessage,
    TrackPluginMovedMessage,
    TrackPluginRemovedMessage,
    TrackRemovePluginMessage,
    TrackSetColorMessage,
    TrackSetGainMessage,
    TrackSetMuteMessage,
    TrackSetNameMessage,
    TrackSetPanMessage,
    TrackSetSoloMessage,
    TrackSoloChangedMessage, TrackMovePluginMessage, TrackAddPluginMessage, TrackAddSendNodeMessage,
    TrackMoveSendNodeMessage, TrackSendAddedMessage, TrackSendMovedMessage, TrackSendRemovedMessage,
    TrackRemoveSendNodeMessage, TrackAddSendsListenerMessage
} from "../api/messages/track-messages";
import OSCMessage from "../osc/osc-message";
import ValidatableOSCMessage from "../api/messages/validatable-osc-message";
import {UUID} from "../utility";
import {AddressWithTypeId} from "../api/api-core";
import {ArrayExtended, ObjectArray} from "../array-extended";
import {Comparable} from "../comparable";
import {Removable} from "../removable";
import {APIConsumer} from "../api/api-consumer";
import TrackPluginModel from "./track-plugin-model";
import Color from "../helpers/color/color";
import {
    AudioCoreAvailableInputSourcesListMessage,
    AudioCoreAvailableOutputReceiversListMessage,
    AudioCoreGetAvailableInputSourcesListMessage,
    AudioCoreGetAvailableOutputReceiversListMessage,
    AvailableMixerNodeInfo
} from "../api/messages/audio-core-messages";
import TrackSendModel from "./track-send-model";

export interface TrackModelControlsListener {
    gainChanged(newValue: number): void;
    panChanged(newValue: number): void;
    muteChanged(newState: boolean): void;
    soloChanged(newState: boolean): void;
}

export interface TrackModelInfoListener {
    nameChanged(newName: string): void;
    colorChanged(newColor: Color): void;
    trackWillBeDeleted(): void;
}

export interface TrackModelPluginListener {
    pluginAdded(plugin: TrackPluginModel, position: number): void;
    pluginRemoved(plugin: TrackPluginModel): void;
    pluginMoved(pluginId: UUID, oldPosition: number, newPosition: number): void;
}

export interface TrackModelSendListener {
    sendAdded(send: TrackSendModel, position: number): void;
    sendRemoved(send: TrackSendModel): void;
    sendMoved(sendId: UUID, oldPosition: number, newPosition: number): void;
}

export interface TrackModelSignalLevelListener {
    signalLevelsChanged(levels: [number, number], isClipping: boolean): void;
}

export default class TrackModel extends APIConsumer implements Comparable, Removable {
    protected id: UUID;
    protected type: number;
    protected gain: number;
    protected pan: number;
    protected mute: boolean;
    protected solo: boolean;
    protected name: string;
    protected color: Color;
    protected plugins: ObjectArray<TrackPluginModel> = new ObjectArray();
    protected sends: ObjectArray<TrackSendModel> = new ObjectArray();
    protected signalLevels: [number, number] = [0, 0];
    protected isClipping: boolean = false;

    protected controlsListeners: ArrayExtended<TrackModelControlsListener> = new ArrayExtended();
    protected infoListeners: ArrayExtended<TrackModelInfoListener> = new ArrayExtended();
    protected pluginListeners: ArrayExtended<TrackModelPluginListener> = new ArrayExtended();
    protected sendListeners: ArrayExtended<TrackModelSendListener> = new ArrayExtended();
    protected signalLevelListeners: ArrayExtended<TrackModelSignalLevelListener> = new ArrayExtended();

    protected static readonly MessageType = {
        GainChanged: 1,
        PanChanged: 2,
        MuteChanged: 3,
        SoloChanged: 4,
        NameChanged: 5,
        ColorChanged: 6,
        PluginAdded: 7,
        PluginRemoved: 8,
        PluginMoved: 9,
        SendAdded: 10,
        SendRemoved: 11,
        SendMoved: 12
    };

    public static readonly AudioSourcePosition = {
        PrePlugins: 0,
        PreFader: 1000,
        PostFader: 1001
    };

    public static readonly Type = {
        AudioTrack: 1,
        FxTrack: 2,
        MixBusTrack: 3,
        OutputBusTrack: 4
    };

    constructor(trackId: UUID, trackType: number) {
        super();
        this.id = trackId;
        this.type = trackType;
        this.initializeAPIConsumer();
    }

    public instanceWillBeDeleted(): void {
        this.terminateAPIConsumer();

        this.infoListeners.forEach((listener) => listener.trackWillBeDeleted());
        this.plugins.forEach((plugin) => plugin.instanceWillBeDeleted());
    }

    public equals(other: Comparable): boolean {
        if (other instanceof TrackModel) {
            return other.getId() === this.id;
        }

        return false;
    }

    public static loadFromAPI(trackId: UUID): Promise<TrackModel> {
        return new Promise((resolve, reject) => {
            window.WOAP.ServiceLocator.getAPIWorker()
                .request(new TrackGetDetailMessage(trackId), TrackDetailMessage.getAddress(trackId))
                .then((message: OSCMessage) => {
                    let msg = new TrackDetailMessage(trackId, message);
                    if (!msg.isValid()) {
                        reject();
                        return;
                    }

                    let track = new TrackModel(trackId, msg.getTrackType());
                    track
                        .setGain(msg.getGainValue(), false)
                        .setPan(msg.getPanValue(), false)
                        .setMute(msg.getMuteState(), false)
                        .setSolo(msg.getSoloState(), false)
                        .setName(msg.getName(), false)
                        .setColor(msg.getColor(), false);

                    ArrayExtended.forEach(msg.getPluginIds(), (pluginId) => {
                        track.loadPluginById(pluginId, -1);
                    });

                    ArrayExtended.forEach(msg.getSendNodeIds(), (sendId) => {
                        track.loadSendById(sendId, -1);
                    });

                    resolve(track);
                }).catch(reject);
        });
    }

    public getId(): UUID {
        return this.id;
    }

    public getType(): number {
        return this.type;
    }

    public setId(newId: UUID): this {
        this.id = newId;
        return this;
    }

    public getGain(): number {
        return this.gain;
    }

    public setGain(newValue: number, notifyAPI: boolean = true): this {
        if (this.gain !== newValue) {
            this.gain = newValue;
            this.controlsListeners.forEach((listener) => listener.gainChanged(newValue));

            if (notifyAPI) {
                this.sendAPIMessage(new TrackSetGainMessage(this.id).addArguments(newValue));
            }
        }

        return this;
    }

    public getPan(): number {
        return this.pan;
    }

    public setPan(newValue: number, notifyAPI: boolean = true): this {
        if (this.pan !== newValue) {
            this.pan = newValue;
            this.controlsListeners.forEach((listener) => listener.panChanged(newValue));

            if (notifyAPI) {
                this.sendAPIMessage(new TrackSetPanMessage(this.id).addArguments(newValue));
            }
        }

        return this;
    }

    public getMute(): boolean {
        return this.mute;
    }

    public setMute(newState: boolean, notifyAPI: boolean = true): this {
        if (this.mute !== newState) {
            this.mute = newState;
            this.controlsListeners.forEach((listener) => listener.muteChanged(newState));

            if (notifyAPI) {
                this.sendAPIMessage(new TrackSetMuteMessage(this.id).addArguments(newState));
            }
        }

        return this;
    }

    public getSolo(): boolean {
        return this.solo;
    }

    public setSolo(newState: boolean, notifyAPI: boolean = true): this {
        if (this.solo !== newState) {
            this.solo = newState;
            this.controlsListeners.forEach((listener) => listener.soloChanged(newState));

            if (notifyAPI) {
                this.sendAPIMessage(new TrackSetSoloMessage(this.id).addArguments(newState));
            }
        }

        return this;
    }

    public getName(): string {
        return this.name;
    }

    public setName(newName: string, notifyAPI: boolean = true): this {
        if (this.name !== newName) {
            this.name = newName;
            this.infoListeners.forEach((listener) => listener.nameChanged(newName));

            if (notifyAPI) {
                this.sendAPIMessage(new TrackSetNameMessage(this.id).addArguments(newName));
            }
        }

        return this;
    }

    public getColor(): Color {
        return this.color;
    }

    public setColor(newColor: Color, notifyAPI: boolean = true): this {
        if (this.color !== newColor) {
            this.color = newColor;
            this.infoListeners.forEach((listener) => listener.colorChanged(newColor));

            if (notifyAPI) {
                this.sendAPIMessage(new TrackSetColorMessage(this.id).addArguments(newColor));
            }
        }

        return this;
    }

    public getPlugins(): ObjectArray<TrackPluginModel> {
        return this.plugins;
    }

    public getPluginIds(): UUID[] {
        return this.plugins.map((plugin) => plugin.getId());
    }

    public getPluginById(pluginId: UUID): TrackPluginModel {
        return this.plugins.find((plugin) => plugin.getId() === pluginId);
    }

    public getSends(): ObjectArray<TrackSendModel> {
        return this.sends;
    }

    public getSendIds(): UUID[] {
        return this.sends.map((send) => send.getId());
    }

    public getSendById(sendId: UUID): TrackSendModel {
        return this.sends.find((send) => send.getId() === sendId);
    }

    public addControlsListener(listener: TrackModelControlsListener): this {
        this.controlsListeners.add(listener);
        return this;
    }

    public removeControlsListener(listener: TrackModelControlsListener): this {
        this.controlsListeners.remove(listener);
        return this;
    }

    public addInfoListener(listener: TrackModelInfoListener): this {
        this.infoListeners.add(listener);
        return this;
    }

    public removeInfoListener(listener: TrackModelInfoListener): this {
        this.infoListeners.remove(listener);
        return this;
    }

    public addPluginListener(listener: TrackModelPluginListener): this {
        this.pluginListeners.add(listener);
        return this;
    }

    public removePluginListener(listener: TrackModelPluginListener): this {
        this.pluginListeners.remove(listener);
        return this;
    }

    public addSendListener(listener: TrackModelSendListener): this {
        this.sendListeners.add(listener);
        return this;
    }

    public removeSendListener(listener: TrackModelSendListener): this {
        this.sendListeners.remove(listener);
        return this;
    }

    public addSignalLevelListener(listener: TrackModelSignalLevelListener): this {
        this.signalLevelListeners.add(listener);
        return this;
    }

    public removeSignalLevelListener(listener: TrackModelSignalLevelListener): this {
        this.signalLevelListeners.remove(listener);
        return this;
    }

    protected loadPluginById(pluginId: UUID, positionToAddTo: number): this {
        TrackPluginModel.loadFromAPI(this, pluginId).then((plugin) => {
            this.plugins.insert(positionToAddTo, plugin);
            this.pluginListeners.forEach((listener) => listener.pluginAdded(plugin, positionToAddTo));
        }).catch(() => {
        });

        return this;
    }

    public addPlugin(uid: number): this {
        this.sendAPIMessage(new TrackAddPluginMessage(this.id).addArguments(uid, -1));
        return this;
    }

    public removePlugin(pluginId: UUID, notifyAPI: boolean = true): this {
        let plugin = this.plugins.find((plugin) => plugin.getId() === pluginId);
        if (plugin !== null) {
            this.plugins.remove(plugin);
            this.pluginListeners.forEach((listener) => listener.pluginRemoved(plugin));
            plugin.instanceWillBeDeleted();

            if (notifyAPI) {
                this.sendAPIMessage(new TrackRemovePluginMessage(this.id).addArguments(pluginId));
            }
        }

        return this;
    }

    public movePlugin(pluginId: UUID, newPosition: number, notifyAPI: boolean = true): this {
        let oldPosition = this.plugins.findIndex((plugin) => plugin.getId() === pluginId);
        if (oldPosition === -1) {
            return this;
        }

        this.plugins.moveAtIndex(oldPosition, newPosition);
        this.pluginListeners.forEach((listener) => listener.pluginMoved(pluginId, oldPosition, newPosition));

        if (notifyAPI) {
            this.sendAPIMessage(new TrackMovePluginMessage(this.id).addArguments(pluginId, newPosition));
        }
        return this;
    }

    protected loadSendById(sendId: UUID, positionToAddTo: number): this {
        TrackSendModel.loadFromAPI(this, sendId).then((send) => {
            this.sends.insert(positionToAddTo, send);
            this.sendListeners.forEach((listener) => listener.sendAdded(send, positionToAddTo));
        }).catch(() => {
        });

        return this;
    }

    public addSend(audioSourcePosition: number): this {
        this.sendAPIMessage(new TrackAddSendNodeMessage(this.id).addArguments(audioSourcePosition, -1));
        return this;
    }

    public removeSend(sendId: UUID, notifyAPI: boolean = true): this {
        let send = this.sends.find((send) => send.getId() === sendId);
        if (send !== null) {
            this.sends.remove(send);
            this.sendListeners.forEach((listener) => listener.sendRemoved(send));
            send.instanceWillBeDeleted();

            if (notifyAPI) {
                this.sendAPIMessage(new TrackRemoveSendNodeMessage(this.id).addArguments(sendId));
            }
        }

        return this;
    }

    public moveSend(sendId: UUID, newPosition: number, notifyAPI: boolean = true): this {
        let oldPosition = this.sends.findIndex((send) => send.getId() === sendId);
        if (oldPosition === -1) {
            return this;
        }

        this.sends.moveAtIndex(oldPosition, newPosition);
        this.sendListeners.forEach((listener) => listener.sendMoved(sendId, oldPosition, newPosition));

        if (notifyAPI) {
            this.sendAPIMessage(new TrackMoveSendNodeMessage(this.id).addArguments(sendId, newPosition));
        }
        return this;
    }

    public setSignalLevels(leftChannel: number, rightChannel: number, isClipping: boolean): this {
        this.signalLevels = [leftChannel, rightChannel];
        this.isClipping = isClipping;
        this.signalLevelListeners.forEach((listener: TrackModelSignalLevelListener) => listener.signalLevelsChanged(this.signalLevels, this.isClipping));
        return this;
    }

    public getSignalLevels(): [number, number] {
        return this.signalLevels;
    }

    /**
     * Returns list of input sources
     * [UUID, name, color, active]
     *
     * @returns {Promise<AvailableMixerNodeInfo[]>}
     */
    public fetchAvailableInputSources(): Promise<AvailableMixerNodeInfo[]> {
        return new Promise((resolve, reject) => {
            this.request(new AudioCoreGetAvailableInputSourcesListMessage().addArguments(this.getId()), AudioCoreAvailableInputSourcesListMessage.getAddress())
                .then((message: OSCMessage) => {
                    let msg = new AudioCoreAvailableInputSourcesListMessage(message);
                    if (!msg.isValid()) {
                        reject();
                        return;
                    }

                    resolve(msg.getSources());
                });
        });
    }

    /**
     * Returns list of output receivers
     * [UUID, name, color, active]
     *
     * @returns {Promise<AvailableMixerNodeInfo[]>}
     */
    public fetchAvailableOutputReceivers(): Promise<AvailableMixerNodeInfo[]> {
        return new Promise((resolve, reject) => {
            this.request(new AudioCoreGetAvailableOutputReceiversListMessage().addArguments(this.getId()), AudioCoreAvailableOutputReceiversListMessage.getAddress())
                .then((message: OSCMessage) => {
                    let msg = new AudioCoreAvailableOutputReceiversListMessage(message);
                    if (!msg.isValid()) {
                        reject();
                        return;
                    }

                    resolve(msg.getReceivers());
                });
        });
    }

    public getMessageTypesToListenTo(): AddressWithTypeId[] {
        return [
            [TrackGainChangedMessage.getAddress(this.id), TrackModel.MessageType.GainChanged],
            [TrackPanChangedMessage.getAddress(this.id), TrackModel.MessageType.PanChanged],
            [TrackMuteChangedMessage.getAddress(this.id), TrackModel.MessageType.MuteChanged],
            [TrackSoloChangedMessage.getAddress(this.id), TrackModel.MessageType.SoloChanged],
            [TrackNameChangedMessage.getAddress(this.id), TrackModel.MessageType.NameChanged],
            [TrackColorChangedMessage.getAddress(this.id), TrackModel.MessageType.ColorChanged],
            [TrackPluginAddedMessage.getAddress(this.id), TrackModel.MessageType.PluginAdded],
            [TrackPluginRemovedMessage.getAddress(this.id), TrackModel.MessageType.PluginRemoved],
            [TrackPluginMovedMessage.getAddress(this.id), TrackModel.MessageType.PluginMoved],
            [TrackSendAddedMessage.getAddress(this.id), TrackModel.MessageType.SendAdded],
            [TrackSendRemovedMessage.getAddress(this.id), TrackModel.MessageType.SendRemoved],
            [TrackSendMovedMessage.getAddress(this.id), TrackModel.MessageType.SendMoved]
        ];
    }

    public getMessagesToSendOnInit(): ValidatableOSCMessage[] {
        return [
            new TrackAddControlsListenerMessage(this.id),
            new TrackAddInfoListenerMessage(this.id),
            new TrackAddPluginsListenerMessage(this.id),
            new TrackAddSendsListenerMessage(this.id)
        ];
    }

    public oscMessageReceived(message: OSCMessage, messageType: number): void {
        switch (messageType) {
            case TrackModel.MessageType.GainChanged: {
                let msg = new TrackGainChangedMessage(this.id, message);
                if (!msg.isValid())
                    return;

                this.setGain(msg.getNewLevel(), false);
                break;
            }
            case TrackModel.MessageType.PanChanged: {
                let msg = new TrackPanChangedMessage(this.id, message);
                if (!msg.isValid())
                    return;

                this.setPan(msg.getNewLevel(), false);
                break;
            }
            case TrackModel.MessageType.MuteChanged: {
                let msg = new TrackMuteChangedMessage(this.id, message);
                if (!msg.isValid())
                    return;

                this.setMute(msg.getNewState(), false);
                break;
            }
            case TrackModel.MessageType.SoloChanged: {
                let msg = new TrackSoloChangedMessage(this.id, message);
                if (!msg.isValid())
                    return;

                this.setSolo(msg.getNewState(), false);
                break;
            }
            case TrackModel.MessageType.NameChanged: {
                let msg = new TrackNameChangedMessage(this.id, message);
                if (!msg.isValid())
                    return;

                this.setName(msg.getNewName(), false);
                break;
            }
            case TrackModel.MessageType.ColorChanged: {
                let msg = new TrackColorChangedMessage(this.id, message);
                if (!msg.isValid())
                    return;

                this.setColor(msg.getNewColor(), false);
                break;
            }
            case TrackModel.MessageType.PluginAdded: {
                let msg = new TrackPluginAddedMessage(this.id, message);
                if (!msg.isValid())
                    return;

                this.loadPluginById(msg.getPluginId(), msg.getPluginPosition());
                break;
            }
            case TrackModel.MessageType.PluginRemoved: {
                let msg = new TrackPluginRemovedMessage(this.id, message);
                if (!msg.isValid())
                    return;

                this.removePlugin(msg.getPluginId(), false);
                break;
            }
            case TrackModel.MessageType.PluginMoved: {
                let msg = new TrackPluginMovedMessage(this.id, message);
                if (!msg.isValid())
                    return;

                this.movePlugin(msg.getPluginId(), msg.getNewPosition(), false);
                break;
            }
            case TrackModel.MessageType.SendAdded: {
                let msg = new TrackSendAddedMessage(this.id, message);
                if (!msg.isValid())
                    return;

                this.loadSendById(msg.getSendNodeId(), msg.getSendPosition());
                break;
            }
            case TrackModel.MessageType.SendRemoved: {
                let msg = new TrackSendRemovedMessage(this.id, message);
                if (!msg.isValid())
                    return;

                this.removeSend(msg.getSendNodeId(), false);
                break;
            }
            case TrackModel.MessageType.SendMoved: {
                let msg = new TrackSendMovedMessage(this.id, message);
                if (!msg.isValid())
                    return;

                this.moveSend(msg.getSendNodeId(), msg.getNewPosition(), false);
                break;
            }
            default:
                break;
        }
    }
}