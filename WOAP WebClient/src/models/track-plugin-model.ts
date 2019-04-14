import {
    PluginActiveChangedMessage,
    PluginAddActiveListenerMessage,
    PluginAddBypassListenerMessage,
    PluginAddListenerMessage,
    PluginAddParameterListenerMessage,
    PluginBypassChangedMessage,
    PluginDetailMessage,
    PluginGetDetailMessage,
    PluginGetParameters,
    PluginGetParameterValues,
    PluginNameChangedMessage,
    PluginParameters,
    PluginParameterValueChangedMessage,
    PluginParameterValues,
    PluginRemoveParameterListenerMessage,
    PluginSetActiveMessage,
    PluginSetBypassMessage,
    PluginSetNameMessage
} from "../api/messages/plugin-messages";
import OSCMessage from "../osc/osc-message";
import {UUID} from "../utility";
import {Comparable} from "../comparable";
import {Removable} from "../removable";
import {APIConsumer} from "../api/api-consumer";
import {AddressWithTypeId} from "../api/api-core";
import ValidatableOSCMessage from "../api/messages/validatable-osc-message";
import {ArrayExtended} from "../array-extended";
import PluginParameterModel from "./plugin-parameter-model";
import PluginViewModel from "./plugin-view-model";
import {PluginExternalListener, PluginExternalListenerInterface} from "./plugin-external-listener";
import PNGImage from "../components/png-image";
import TrackModel from "./track-model";
import {ConfigurationManager} from "../configuration-manager";

export interface TrackPluginModelListener {
    nameChanged(newName: string): void;

    bypassChanged(newState: boolean): void;

    activeChanged(newState: boolean): void;

    pluginWillBeDeleted(): void;
}

export default class TrackPluginModel extends APIConsumer implements Comparable, Removable {
    protected id: UUID;
    protected name: string;
    protected ref: string;
    protected typeId: number;
    protected bypass: boolean;
    protected active: boolean;
    protected parameters: ArrayExtended<PluginParameterModel> = new ArrayExtended<PluginParameterModel>();
    protected views: ArrayExtended<PluginViewModel> = new ArrayExtended<PluginViewModel>();
    protected images: ArrayExtended<PNGImage> = new ArrayExtended<PNGImage>();
    protected track: TrackModel;
    protected script: string = "";
    protected externalListener: PluginExternalListenerInterface = new PluginExternalListener(this);

    protected numParameterListeners: number = 0;
    protected parametersFetched: boolean = false;
    protected parametersListenerAdded: boolean = false;
    protected listeners: ArrayExtended<TrackPluginModelListener> = new ArrayExtended();

    public static readonly MessageType = {
        NameChanged: 1,
        BypassChanged: 2,
        ActiveChanged: 3,
        ParameterValueChanged: 4
    };

    constructor(track: TrackModel, pluginId: UUID, pluginRef: string, pluginTypeId: number) {
        super();
        this.track = track;
        this.id = pluginId;
        this.ref = pluginRef;
        this.typeId = pluginTypeId;
        this.initializeAPIConsumer();
    }

    public instanceWillBeDeleted(): void {
        this.terminateAPIConsumer();
        this.listeners.forEach((listener) => listener.pluginWillBeDeleted());
    }

    public equals(other: Comparable): boolean {
        if (other instanceof TrackPluginModel) {
            return other.getId() === this.id;
        }

        return false;
    }

    public static loadFromAPI(track: TrackModel, pluginId: UUID): Promise<TrackPluginModel> {
        return new Promise<TrackPluginModel>((resolve, reject) => {
            window.WOAP.ServiceLocator.getAPIWorker()
                .request(new PluginGetDetailMessage(pluginId), PluginDetailMessage.getAddress(pluginId))
                .then((message: OSCMessage) => {
                    let msg = new PluginDetailMessage(pluginId, message);
                    if (!msg.isValid()) {
                        reject();
                        return;
                    }

                    let instance = new TrackPluginModel(track, pluginId, msg.getRef(), msg.getTypeId())
                        .setName(msg.getName(), false)
                        .setBypass(msg.getBypassedState(), false)
                        .setActive(msg.getActiveState(), false);

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
            this.externalListener.nameChanged(newName);

            if (notifyAPI) {
                this.sendAPIMessage(new PluginSetNameMessage(this.id).addArguments(newName));
            }
        }

        return this;
    }

    public getRef(): string {
        return this.ref;
    }

    public getTypeId(): number {
        return this.typeId;
    }

    public getBypass(): boolean {
        return this.bypass;
    }

    public setBypass(newState: boolean, notifyAPI: boolean = true) {
        if (newState !== this.bypass) {
            this.bypass = newState;
            this.listeners.forEach((listener) => listener.bypassChanged(newState));
            this.externalListener.bypassChanged(newState);

            if (notifyAPI) {
                this.sendAPIMessage(new PluginSetBypassMessage(this.id).addArguments(newState));
            }
        }

        return this;
    }

    public getActive(): boolean {
        return this.active;
    }

    public setActive(newState: boolean, notifyAPI: boolean = true): this {
        if (newState !== this.active) {
            this.active = newState;
            this.listeners.forEach((listener) => listener.activeChanged(newState));
            this.externalListener.activeChanged(newState);

            if (notifyAPI) {
                this.sendAPIMessage(new PluginSetActiveMessage(this.id).addArguments(newState));
            }
        }

        return this;
    }

    public addView(view: PluginViewModel): this {
        this.views.add(view);
        return this;
    }

    public hasViews(): boolean {
        return this.views.isNotEmpty();
    }

    public getViews(): PluginViewModel[] {
        return this.views.toArray();
    }

    public getDefaultView(): PluginViewModel {
        if (this.views.isEmpty()) {
            return null;
        }

        let defaultView = this.views.find((view) => view.getIsDefault());
        if (defaultView !== null) {
            return defaultView;
        } else {
            return this.views[0];
        }
    }

    public addImage(image: PNGImage): this {
        this.images.add(image);
        return this;
    }

    public getImages(): PNGImage[] {
        return this.images.toArray();
    }

    public getImageByRef(ref: string): PNGImage {
        return this.images.find((image) => image.getRef() === ref);
    }

    public setScript(script: string): this {
        this.script = script;
        return this;
    }

    public getScript(): string {
        return this.script;
    }

    public getTrack(): TrackModel {
        return this.track;
    }

    public interestedInParameterValues(): Promise<void> {
        ++this.numParameterListeners;
        if (!this.parametersFetched) {
            return this.request(new PluginGetParameters(this.id), PluginParameters.getAddress(this.id))
                .then((message: OSCMessage) => {
                    let msg = new PluginParameters(this.id, message);
                    if (!msg.isValid())
                        throw new Error();

                    for (let paramInfo of msg.getParameters()) {
                        let parameter = new PluginParameterModel(this.id, paramInfo[0], paramInfo[1]);
                        parameter.setValueAndText(paramInfo[2], paramInfo[3]);
                        this.parameters.add(parameter);
                    }
                    this.parametersFetched = true;

                    return this.addParameterListeners();
                });
        } else {
            return this.addParameterListeners();
        }
    }

    protected addParameterListeners(): Promise<any> {
        return new Promise<void>((resolve, reject) => {
            if (this.parametersListenerAdded) {
                resolve();
            }

            this.sendAPIMessage(new PluginAddParameterListenerMessage(this.id));
            this.request(new PluginGetParameterValues(this.id), PluginParameterValues.getAddress(this.id)).then((message: OSCMessage) => {
                let msg = new PluginParameterValues(this.id, message);
                if (!msg.isValid())
                    throw new Error();

                let values = msg.getParameterValues();
                if (values.length !== this.parameters.getLength())
                    throw new Error();

                for (let i = 0; i < values.length; ++i) {
                    this.parameters.getUnchecked(i).setValueAndText(values[i][0], values[i][1]);
                }
                this.parametersListenerAdded = true;
                resolve();
            }).catch(reject);
        });
    }

    public noLongerInterestedInParameterValues(): void {
        if (this.numParameterListeners > 0) {
            --this.numParameterListeners;

            if (this.numParameterListeners === 0) {
                this.sendAPIMessage(new PluginRemoveParameterListenerMessage(this.id));
                this.parametersListenerAdded = false;
            }
        }
    }

    public getParameterByRef(ref: string): PluginParameterModel {
        return this.parameters.find((parameter) => parameter.getRef() === ref);
    }

    public pluginViewCreated(): void {
        this.externalListener.viewCreated();
    }

    public pluginViewWillBeDeleted(): void {
        this.externalListener.viewWillBeDeleted();
        this.removeExternalListener();
    }

    public getParameterUnchecked(index: number): PluginParameterModel {
        return this.parameters.getUnchecked(index);
    }

    public getParameters(): PluginParameterModel[] {
        return this.parameters.toArray();
    }

    public setExternalListener(newExternalListener: PluginExternalListenerInterface): this {
        let listener = new PluginExternalListener(this);

        for (let func of PluginExternalListener.overridableFunctions) {
            if (typeof newExternalListener[func] === "function") {
                listener[func] = newExternalListener[func];
            }
        }

        this.externalListener = listener;
        return this;
    }

    public removeExternalListener(): this {
        this.externalListener = new PluginExternalListener(this);
        return this;
    }

    public addListener(listener: TrackPluginModelListener): this {
        this.listeners.add(listener);
        return this;
    }

    public removeListener(listener: TrackPluginModelListener): this {
        this.listeners.remove(listener);
        return this;
    }

    public getMessageTypesToListenTo(): AddressWithTypeId[] {
        return [
            [PluginNameChangedMessage.getAddress(this.id), TrackPluginModel.MessageType.NameChanged],
            [PluginBypassChangedMessage.getAddress(this.id), TrackPluginModel.MessageType.BypassChanged],
            [PluginActiveChangedMessage.getAddress(this.id), TrackPluginModel.MessageType.ActiveChanged],
            [PluginParameterValueChangedMessage.getAddress(this.id), TrackPluginModel.MessageType.ParameterValueChanged]
        ];
    }

    public getMessagesToSendOnInit(): ValidatableOSCMessage[] {
        return [
            new PluginAddListenerMessage(this.id),
            new PluginAddBypassListenerMessage(this.id),
            new PluginAddActiveListenerMessage(this.id)
        ];
    }

    public oscMessageReceived(message: OSCMessage, messageType: number): void {
        switch (messageType) {
            case TrackPluginModel.MessageType.ParameterValueChanged: {
                let msg = new PluginParameterValueChangedMessage(this.id, message);

                if (!msg.isValid())
                    return;

                let parameterIndex = msg.getParameterIndex();
                if (this.parameters.getLength() <= parameterIndex) {
                    if (ConfigurationManager.isDebug()) {
                        console.error("Parameter with index " + parameterIndex + " was not found in " + this.ref + " plugin");
                    }
                    return;
                }

                this.parameters.getUnchecked(parameterIndex).setValueAndText(msg.getNewValue(), msg.getNewValueText());
                this.externalListener.parameterChanged(this.parameters.getUnchecked(parameterIndex), msg.getNewValue(), msg.getNewValueText());
                break;
            }
            case TrackPluginModel.MessageType.BypassChanged: {
                let msg = new PluginBypassChangedMessage(this.id, message);

                if (!msg.isValid())
                    return;

                this.setBypass(msg.getNewState(), false);
                break;
            }
            case TrackPluginModel.MessageType.ActiveChanged: {
                let msg = new PluginActiveChangedMessage(this.id, message);

                if (!msg.isValid())
                    return;

                this.setActive(msg.getNewState(), false);
                break;
            }
            case TrackPluginModel.MessageType.NameChanged: {
                let msg = new PluginNameChangedMessage(this.id, message);

                if (!msg.isValid())
                    return;

                this.setName(msg.getNewName(), false);
                break;
            }
            default:
                break;
        }
    }
}
