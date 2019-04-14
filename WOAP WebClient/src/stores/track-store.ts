import {Utility, UUID} from "../utility";
import {AddressWithTypeId} from "../api/api-core";
import ValidatableOSCMessage from "../api/messages/validatable-osc-message";
import OSCMessage from "../osc/osc-message";
import {ArrayExtended, ObjectArray} from "../array-extended";
import {
    AudioCoreTrackRemovedMessage,
    AudioCoreTrackAddedMessage,
    AudioCoreTracksListMessage,
    AudioCoreGetTracksListMessage,
    AudioCoreRemoveTrackMessage,
    AudioCoreAddTrackMessage,
    AudioCoreAddListenerMessage, AudioCoreTrackMovedMessage, AudioCoreMoveTrackMessage, AudioCoreAddSignalLevelListenerMessage,
    AudioCoreSignalLevelsMessage, AudioCoreConnectionsChangedMessage,
    AudioCoreConnectNodesMessage, AudioCoreRemoveNodesConnectionMessage
} from "../api/messages/audio-core-messages";
import TrackModel from "../models/track-model";
import {ConfigurationManager} from "../configuration-manager";
import Store from "./store";

export interface TrackStoreListener {
    trackAdded(track: TrackModel, position: number): void;
    trackRemoved(track: TrackModel): void;
    trackMoved(trackId: UUID, oldPosition: number, newPosition: number): void;
    trackSelected(track: TrackModel): void;
}

export interface TrackStoreConnectionsListener {
    connectionsChanged(): void;
}

export default class TrackStore extends Store<TrackStoreListener> {
    protected tracks: ObjectArray<TrackModel> = new ObjectArray();
    protected selectedTrack: TrackModel = null;
    protected connectionListeners: ArrayExtended<TrackStoreConnectionsListener> = new ArrayExtended();

    protected static readonly MessageTypeId = {
        TrackAdded: 1,
        TrackRemoved: 2,
        TrackMoved: 3,
        SignalLevels: 4,
        ConnectionsChanged: 5
    };

    public reloadStore(): void {
        this.loadAllTracks();
    }

    public addConnectionsListener(listener: TrackStoreConnectionsListener): this {
        this.connectionListeners.add(listener);
        return this;
    }

    public removeConnectionsListener(listener: TrackStoreConnectionsListener): this {
        this.connectionListeners.remove(listener);
        return this;
    }

    public getTrackIds(): UUID[] {
        let output = [];
        this.tracks.forEach((track) => output.push(track.getId()));
        return output;
    }

    public getTracks(): ObjectArray<TrackModel> {
        return this.tracks;
    }

    public getTrackById(trackId: UUID): TrackModel {
        return this.tracks.find((track) => trackId === track.getId());
    }

    public getSelectedTrack(): TrackModel {
        return this.selectedTrack;
    }

    public selectTrack(track: TrackModel): this {
        if (this.selectedTrack === track)
            return;

        this.selectedTrack = track;
        Utility.setLocalStorageValue("selectedTrack", track === null ? "" : track.getId());

        this.listeners.forEach((listener: TrackStoreListener) => listener.trackSelected(track));
    }

    public addTrackToPosition(trackType: number, position: number = -1): this {
        this.sendAPIMessage(new AudioCoreAddTrackMessage().addArguments(trackType, position));
        return this;
    }

    public addTrack(trackType: number): this {
        let position = -1;

        if (this.selectedTrack !== null) {
            position = this.tracks.findIndex((item) => item === this.selectedTrack) + 1;
        }

        this.sendAPIMessage(new AudioCoreAddTrackMessage().addArguments(trackType, position));
        return this;
    }

    public removeTrack(trackId: UUID, notifyAPI: boolean = true): this {
        let track = this.tracks.find((track) => track.getId() === trackId);
        if (track !== null) {
            if (track === this.selectedTrack) {
                this.selectTrack(null);
            }

            this.tracks.remove(track);
            this.listeners.forEach((listener) => listener.trackRemoved(track));
            track.instanceWillBeDeleted();

            if (notifyAPI) {
                this.sendAPIMessage(new AudioCoreRemoveTrackMessage().addArguments(trackId));
            }
        }

        return this;
    }

    public removeAllTracks(notifyAPI: boolean = true): this {
        this.selectTrack(null);
        while (true) {
            let track = this.tracks.pop();
            if (track === null) {
                break;
            }

            this.listeners.forEach((listener) => listener.trackRemoved(track));
            track.instanceWillBeDeleted();

            if (notifyAPI) {
                this.sendAPIMessage(new AudioCoreRemoveTrackMessage().addArguments(track.getId()));
            }
        }

        return this;
    }

    public moveTrack(trackId: UUID, newPosition: number, notifyAPI: boolean = true): this {
        let oldPosition = this.tracks.findIndex((track) => track.getId() === trackId);
        if (oldPosition === -1) {
            return this;
        }

        this.tracks.moveAtIndex(oldPosition, newPosition);
        this.listeners.forEach((listener) => listener.trackMoved(trackId, oldPosition, newPosition));

        if (notifyAPI) {
            this.sendAPIMessage(new AudioCoreMoveTrackMessage().addArguments(trackId, newPosition));
        }

        return this;
    }

    protected loadTrackById(trackId: UUID, position: number): this {
        TrackModel.loadFromAPI(trackId).then((trackModel) => {
            this.tracks.insert(position, trackModel);
            this.listeners.forEach((listener) => listener.trackAdded(trackModel, position));

            if (trackId === Utility.getLocalStorageValue("selectedTrack")) {
                this.selectTrack(trackModel);
            }
        }).catch(() => {
        });

        return this;
    }

    protected loadAllTracks(): void {
        if (!this.tracks.isEmpty()) {
            this.removeAllTracks(false);
        }

        this.request(new AudioCoreGetTracksListMessage(), AudioCoreTracksListMessage.getAddress())
            .then((message: OSCMessage) => {
                let msg = new AudioCoreTracksListMessage(message);
                if (!msg.isValid()) {
                    return;
                }

                ArrayExtended.forEach(msg.getTrackIds(), (trackId) => {
                    this.loadTrackById(trackId, -1);
                });

                setTimeout(() => {
                    this.sendAPIMessage(new AudioCoreAddSignalLevelListenerMessage().addArguments(ConfigurationManager.getSignalLevelsRefreshRate()));
                }, 1000);
            })
            .catch(() => {
            });
    }

    protected setTracksSignalLevels(numTracks: number, trackLevels: DataView, trackClipData: DataView): void {
        let length = Utility.lowerOf(numTracks, this.tracks.getLength());

        let sampleIndex = 0;
        for (let i = 0; i < length; ++i) {
            this.tracks.getUnchecked(i).setSignalLevels(
                Utility.readSignalLevelSampleFromBlock(trackLevels, sampleIndex),
                Utility.readSignalLevelSampleFromBlock(trackLevels, sampleIndex + 1),
                Utility.readBitFromBlock(trackClipData, i)
            );
            sampleIndex += 2;
        }
    }

    public connect(sourceId: UUID, receiverId: UUID): this {
        this.sendAPIMessage(new AudioCoreConnectNodesMessage().addArguments(sourceId, receiverId));
        return this;
    }

    public removeConnection(sourceId: UUID, receiverId: UUID): this {
        this.sendAPIMessage(new AudioCoreRemoveNodesConnectionMessage().addArguments(sourceId, receiverId));
        return this;
    }

    public oscMessageReceived(message: OSCMessage, messageType: number): void {
        switch (messageType) {
            case TrackStore.MessageTypeId.TrackAdded: {
                let msg = new AudioCoreTrackAddedMessage(message);
                if (!msg.isValid())
                    return;

                this.loadTrackById(msg.getTrackId(), msg.getTrackPosition());
                break;
            }
            case TrackStore.MessageTypeId.TrackRemoved: {
                let msg = new AudioCoreTrackRemovedMessage(message);
                if (!msg.isValid())
                    return;

                this.removeTrack(msg.getTrackId(), false);
                break;
            }
            case TrackStore.MessageTypeId.TrackMoved: {
                let msg = new AudioCoreTrackMovedMessage(message);
                if (!msg.isValid())
                    return;

                this.moveTrack(msg.getTrackId(), msg.getNewTrackPosition(), false);
                break;
            }
            case TrackStore.MessageTypeId.SignalLevels: {
                let msg = new AudioCoreSignalLevelsMessage(message);
                if (!msg.isValid())
                    return;

                this.setTracksSignalLevels(msg.getNumTracks(), msg.getTracksSignalLevels(), msg.getTrackClipData());
                break;
            }
            case TrackStore.MessageTypeId.ConnectionsChanged: {
                let msg = new AudioCoreConnectionsChangedMessage(message);
                if (!msg.isValid())
                    return;

                this.connectionListeners.forEach((listener) => listener.connectionsChanged());
                break;
            }
            default:
                break;
        }
    }

    public getMessageTypesToListenTo(): AddressWithTypeId[] {
        return [
            [AudioCoreSignalLevelsMessage.getAddress(), TrackStore.MessageTypeId.SignalLevels],
            [AudioCoreTrackAddedMessage.getAddress(), TrackStore.MessageTypeId.TrackAdded],
            [AudioCoreTrackRemovedMessage.getAddress(), TrackStore.MessageTypeId.TrackRemoved],
            [AudioCoreTrackMovedMessage.getAddress(), TrackStore.MessageTypeId.TrackMoved],
            [AudioCoreConnectionsChangedMessage.getAddress(), TrackStore.MessageTypeId.ConnectionsChanged],
        ];
    }

    public getMessagesToSendOnInit(): ValidatableOSCMessage[] {
        return [
            new AudioCoreAddListenerMessage()
        ];
    }
}