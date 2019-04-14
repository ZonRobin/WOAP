import {APIConsumer} from "../api/api-consumer";
import {ArrayExtended} from "../array-extended";

export interface IStore {
    reloadStore(): void;
    initialize(): void;
}

export default abstract class Store<ListenerType> extends APIConsumer implements IStore {

    protected listeners: ArrayExtended<ListenerType> = new ArrayExtended();

    private storeInitialized: boolean = false;

    public abstract reloadStore(): void;

    public initialize(): void {
        if (!this.storeInitialized) {
            this.initializeAPIConsumer();
            this.storeInitialized = true;
        } else {
            this.terminateAPIConsumer();
            this.initializeAPIConsumer();
        }

        this.reloadStore();
    }

    public terminate(): void {
        this.terminateAPIConsumer();
    }

    public addListener(listener: ListenerType): this {
        this.listeners.add(listener);
        return this;
    }

    public removeListener(listener: ListenerType): this {
        this.listeners.remove(listener);
        return this;
    }
}