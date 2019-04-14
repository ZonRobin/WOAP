import TrackStore from "./track-store";
import SystemStore from "./system-store";
import {ArrayExtended} from "../array-extended";
import PluginStore from "./plugin-store";
import {IStore} from "./store";

export default class StoreManager {
    protected stores: ArrayExtended<IStore> = new ArrayExtended();

    public readonly TrackStore: TrackStore;
    public readonly SystemStore: SystemStore;
    public readonly PluginStore: PluginStore;

    constructor() {
        this.stores
            .add(this.TrackStore = new TrackStore())
            .add(this.SystemStore = new SystemStore())
            .add(this.PluginStore = new PluginStore());
    }

    public reloadStores(initialize: boolean = false): this {
        this.stores.forEach((store) => {
            store.initialize();
        });

        return this;
    }
}