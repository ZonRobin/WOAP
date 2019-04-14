import OSCMessage from "../osc/osc-message";
import {UUID} from "../utility";
import {Comparable} from "../comparable";
import {Removable} from "../removable";
import {APIConsumer} from "../api/api-consumer";
import {AddressWithTypeId} from "../api/api-core";
import ValidatableOSCMessage from "../api/messages/validatable-osc-message";
import {ArrayExtended} from "../array-extended";
import TrackModel from "./track-model";
import {
    SendAddAudioSourcePositionListenerMessage,
    SendAddControlsListenerMessage,
    SendAddInfoListenerMessage,
    SendAudioSourcePositionChangedMessage, SendColorChangedMessage, SendDetailMessage,
    SendGainChangedMessage, SendGetDetailMessage,
    SendNameChangedMessage, SendPanChangedMessage,
    SendSetAudioSourcePositionMessage, SendSetGainMessage, SendSetNameMessage,
    SendSetPanMessage
} from "../api/messages/track-send-node-messages";
import Color from "../helpers/color/color";

export interface TrackSendModelListener {
    colorChanged(newColor: Color): void;
    nameChanged(newName: string): void;
    gainChanged(newGain: number): void;
    panChanged(newPan: number): void;
    audioSourcePositionChanged(newPosition: number): void;
    sendWillBeDeleted(): void;
}

export default class TrackSendModel extends APIConsumer implements Comparable, Removable {
    protected id: UUID;
    protected name: string;
    protected color: Color;
    protected gain: number;
    protected pan: number;
    protected audioSourcePosition: number;
    protected track: TrackModel;

    protected listeners: ArrayExtended<TrackSendModelListener> = new ArrayExtended();

    public static readonly MessageType = {
        NameChanged: 1,
        ColorChanged: 2,
        GainChanged: 3,
        PanChanged: 4,
        AudioSourcePositionChanged: 5
    };

    constructor(track: TrackModel, sendId: UUID) {
        super();
        this.track = track;
        this.id = sendId;
        this.initializeAPIConsumer();
    }

    public instanceWillBeDeleted(): void {
        this.terminateAPIConsumer();
        this.listeners.forEach((listener) => listener.sendWillBeDeleted());
    }

    public equals(other: Comparable): boolean {
        if (other instanceof TrackSendModel) {
            return other.getId() === this.id;
        }

        return false;
    }

    public static loadFromAPI(track: TrackModel, sendId: UUID): Promise<TrackSendModel> {
        return new Promise<TrackSendModel>((resolve, reject) => {
            window.WOAP.ServiceLocator.getAPIWorker()
                .request(new SendGetDetailMessage(sendId), SendDetailMessage.getAddress(sendId))
                .then((message: OSCMessage) => {
                    let msg = new SendDetailMessage(sendId, message);
                    if (!msg.isValid()) {
                        reject();
                        return;
                    }

                    let instance = new TrackSendModel(track, sendId)
                        .setName(msg.getName(), false)
                        .setColor(msg.getColor())
                        .setGain(msg.getGainValue(), false)
                        .setPan(msg.getPanValue(), false)
                        .setAudioSourcePosition(msg.getAudioSourcePosition(), false);

                    resolve(instance);
                }).catch(reject);
        });
    }

    public getId(): UUID {
        return this.id;
    }

    public getName(): string {
        return this.name;
    }

    public setName(newName: string, notifyAPI: boolean = true): this {
        if (newName !== this.name) {
            this.name = newName;
            this.listeners.forEach((listener) => listener.nameChanged(newName));

            if (notifyAPI) {
                this.sendAPIMessage(new SendSetNameMessage(this.id).addArguments(newName));
            }
        }

        return this;
    }

    public getColor(): Color {
        return this.color;
    }

    protected setColor(newColor: Color): this {
        if (newColor !== this.color) {
            this.color = newColor;
            this.listeners.forEach((listener) => listener.colorChanged(newColor));
        }

        return this;
    }

    public getGain(): number {
        return this.gain;
    }

    public setGain(newValue: number, notifyAPI: boolean = true) {
        if (newValue !== this.gain) {
            this.gain = newValue;
            this.listeners.forEach((listener) => listener.gainChanged(newValue));

            if (notifyAPI) {
                this.sendAPIMessage(new SendSetGainMessage(this.id).addArguments(newValue));
            }
        }

        return this;
    }

    public getPan(): number {
        return this.pan;
    }

    public setPan(newValue: number, notifyAPI: boolean = true) {
        if (newValue !== this.pan) {
            this.pan = newValue;
            this.listeners.forEach((listener) => listener.panChanged(newValue));

            if (notifyAPI) {
                this.sendAPIMessage(new SendSetPanMessage(this.id).addArguments(newValue));
            }
        }

        return this;
    }

    public getAudioSourcePosition(): number {
        return this.audioSourcePosition;
    }

    public setAudioSourcePosition(newAudioSourcePosition: number, notifyAPI: boolean = true) {
        if (newAudioSourcePosition !== this.audioSourcePosition) {
            this.audioSourcePosition = newAudioSourcePosition;
            this.listeners.forEach((listener) => listener.audioSourcePositionChanged(newAudioSourcePosition));

            if (notifyAPI) {
                this.sendAPIMessage(new SendSetAudioSourcePositionMessage(this.id).addArguments(newAudioSourcePosition));
            }
        }

        return this;
    }

    public getTrack(): TrackModel {
        return this.track;
    }

    public addListener(listener: TrackSendModelListener): this {
        this.listeners.add(listener);
        return this;
    }

    public removeListener(listener: TrackSendModelListener): this {
        this.listeners.remove(listener);
        return this;
    }

    public getMessageTypesToListenTo(): AddressWithTypeId[] {
        return [
            [SendNameChangedMessage.getAddress(this.id), TrackSendModel.MessageType.NameChanged],
            [SendColorChangedMessage.getAddress(this.id), TrackSendModel.MessageType.ColorChanged],
            [SendGainChangedMessage.getAddress(this.id), TrackSendModel.MessageType.GainChanged],
            [SendPanChangedMessage.getAddress(this.id), TrackSendModel.MessageType.PanChanged],
            [SendAudioSourcePositionChangedMessage.getAddress(this.id), TrackSendModel.MessageType.AudioSourcePositionChanged]
        ];
    }

    public getMessagesToSendOnInit(): ValidatableOSCMessage[] {
        return [
            new SendAddInfoListenerMessage(this.id),
            new SendAddControlsListenerMessage(this.id),
            new SendAddAudioSourcePositionListenerMessage(this.id)
        ];
    }

    public oscMessageReceived(message: OSCMessage, messageType: number): void {
        switch (messageType) {
            case TrackSendModel.MessageType.NameChanged: {
                let msg = new SendNameChangedMessage(this.id, message);
                if (!msg.isValid())
                    return;

                this.setName(msg.getNewName(), false);
                break;
            }
            case TrackSendModel.MessageType.ColorChanged: {
                let msg = new SendColorChangedMessage(this.id, message);
                if (!msg.isValid())
                    return;

                this.setColor(msg.getNewColor());
                break;
            }
            case TrackSendModel.MessageType.GainChanged: {
                let msg = new SendGainChangedMessage(this.id, message);
                if (!msg.isValid())
                    return;

                this.setGain(msg.getNewLevel(), false);
                break;
            }
            case TrackSendModel.MessageType.PanChanged: {
                let msg = new SendPanChangedMessage(this.id, message);
                if (!msg.isValid())
                    return;

                this.setPan(msg.getNewLevel(), false);
                break;
            }
            case TrackSendModel.MessageType.AudioSourcePositionChanged: {
                let msg = new SendAudioSourcePositionChangedMessage(this.id, message);
                if (!msg.isValid())
                    return;

                this.setAudioSourcePosition(msg.getNewAudioSourcePosition(), false);
                break;
            }
            default:
                break;
        }
    }
}
