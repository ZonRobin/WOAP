import {ArrayExtended} from "../array-extended";
import PluginPrototype from "../models/plugin-prototype";
import PromiseOnce from "../promise-once";
import {Utility} from "../utility";
import {
    PluginManagerAddListenerMessage,
    PluginManagerGetPluginTypesMessage,
    PluginManagerPluginTypeAddedMessage,
    PluginManagerPluginTypesMessage,
    PluginManagerPluginTypesScannedMessage,
    PluginTypeInfo
} from "../api/messages/plugin-manager-messages";
import OSCMessage from "../osc/osc-message";
import ValidatableOSCMessage from "../api/messages/validatable-osc-message";
import Store from "./store";

export interface PluginStoreListener {
    pluginTypesChanged(): void;
}

export interface PluginConfigParser {
    parseFile(path: string): Promise<PluginPrototype>;
}

export default class PluginStore extends Store<PluginStoreListener> {
    protected static readonly MessageTypeId = {
        PluginsScanned: 1,
        PluginTypeAdded: 2
    };
    protected pluginPrototypes: ArrayExtended<PluginPrototype> = new ArrayExtended();
    protected pluginPrototypePromises: Array<PromiseOnce<PluginPrototype>> = [];
    protected pluginTypesPromise: PromiseOnce<PluginTypeInfo[]>;
    protected pluginTypes: PluginTypeInfo[] = [];

    public constructor() {
        super();
        this.pluginTypesPromise = new PromiseOnce<PluginTypeInfo[]>((resolve, reject) => {
            this.request(new PluginManagerGetPluginTypesMessage(), PluginManagerPluginTypesMessage.getAddress())
                .then((message: OSCMessage) => {
                    let msg = new PluginManagerPluginTypesMessage(message);
                    if (!msg.isValid()) {
                        reject();
                        return;
                    }

                    this.pluginTypes = msg.getPluginTypes();
                    resolve(this.pluginTypes);
                });
        });
    }

    public getPluginPrototype(pluginUid: number, parser: PluginConfigParser): Promise<PluginPrototype> {
        let pluginUidStr = pluginUid + "";
        let promise = this.pluginPrototypePromises[pluginUidStr];
        if (Utility.isUndefined(promise)) {

            promise = new PromiseOnce<PluginPrototype>((resolve, reject) => {
                parser.parseFile("pag-templates/pag-" + pluginUidStr + ".xml").then((prototype: PluginPrototype) => {
                    this.pluginPrototypes.add(prototype);
                    resolve(prototype);
                }).catch(reject);
            });
            this.pluginPrototypePromises[pluginUidStr] = promise;
        }

        return promise.requestOrReturnCb(() => this.pluginPrototypes.find((item: PluginPrototype) => pluginUid === item.getId()));
    }

    public fetchPluginTypes(): Promise<PluginTypeInfo[]> {
        return this.pluginTypesPromise.requestOrReturn(this.pluginTypes);
    }

    public oscMessageReceived(message: OSCMessage, messageType: number): void {
        switch (messageType) {
            case PluginStore.MessageTypeId.PluginsScanned: {
                let msg = new PluginManagerPluginTypesScannedMessage(message);
                if (!msg.isValid())
                    return;

                this.pluginTypesPromise.reset();
                this.pluginTypes = [];
                this.listeners.forEach((listener) => listener.pluginTypesChanged());
                break;
            }
            case PluginStore.MessageTypeId.PluginTypeAdded: {
                let msg = new PluginManagerPluginTypeAddedMessage(message);
                if (!msg.isValid())
                    return;

                this.pluginTypes.push([msg.getPluginTypeUid(), msg.getPluginTypeName(), msg.getPluginTypeManufacturerName()]);
                this.listeners.forEach((listener) => listener.pluginTypesChanged());
                break;
            }
            default:
                break;
        }
    }

    public getMessagesToSendOnInit(): ValidatableOSCMessage[] {
        return [
            new PluginManagerAddListenerMessage()
        ];
    }

    public reloadStore(): void {
        this.pluginTypes = [];
        this.pluginTypesPromise.reset();
    }
}