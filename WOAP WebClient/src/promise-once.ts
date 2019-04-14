import PromiseCallbacks from "./promise-callbacks";

export enum PromiseOneState {
    NonRequested,
    Processing,
    Resolved,
    Rejected
}

export default class PromiseOnce<ReturnType> {
    protected promise: Promise<ReturnType>;
    protected executor: (resolve: (value?: ReturnType | PromiseLike<ReturnType>) => void, reject: (reason?: any) => void) => void;
    protected state: PromiseOneState = PromiseOneState.NonRequested;
    protected data: any;
    protected pendingPromises: Array<PromiseCallbacks<ReturnType>> = [];

    /**
     * Creates new instance. To call executor callback, call request()
     *
     * @param executor
     */
    constructor(executor: (resolve: (value?: ReturnType | PromiseLike<ReturnType>) => void, reject: (reason?: any) => void) => void) {
        this.executor = executor;
    }

    /**
     * Forces to call the Promise again.
     * If there"s a Promise in process it waits and calls new Promise after return of the current processing Promise.
     *
     * @returns {Promise<ReturnType>}
     */
    public retry(): Promise<ReturnType> {
        if (this.state === PromiseOneState.Processing) {
            return new Promise<ReturnType>((resolve, reject) => {
                /* First request() adds to que, waits for current process */
                this.request().then(() => {
                    this.request()
                        .then((result) => resolve(result))
                        .catch((error) => reject(error));
                }).catch(() => {
                    this.request()
                        .then((result) => resolve(result))
                        .catch((error) => reject(error));
                });
            });
        } else {
            this.reset();
            return this.request();
        }
    }

    /**
     * Calls the Promise executor or returns data if the Promise was already called.
     * You can call this multiple times but the executor is called only once.
     *
     * @returns {Promise<ReturnType>}
     */
    public request(): Promise<ReturnType> {
        switch (this.state) {
            case PromiseOneState.Resolved:
                return new Promise<ReturnType>((resolve, reject) => {
                    resolve(this.data);
                });
            case PromiseOneState.Rejected:
                return new Promise<ReturnType>((resolve, reject) => {
                    reject(this.data);
                });
            case PromiseOneState.NonRequested:
                this.state = PromiseOneState.Processing;
                return new Promise<ReturnType>((resolve, reject) => {
                    new Promise<ReturnType>(this.executor).then((result) => {
                        this.state = PromiseOneState.Resolved;
                        this.data = result;
                        resolve(result);
                        this.pendingPromises.forEach((callbacks) => callbacks.resolve(result));
                        this.pendingPromises = [];
                    }).catch((error) => {
                        this.state = PromiseOneState.Rejected;
                        this.data = error;
                        reject(error);
                        this.pendingPromises.forEach((callbacks) => callbacks.reject(error));
                        this.pendingPromises = [];
                    });
                });
            case PromiseOneState.Processing:
                return new Promise<ReturnType>((resolve, reject) => {
                    this.pendingPromises.push(new PromiseCallbacks(resolve, reject));
                });
            default:
                throw new Error("Unknown promise state");
        }
    }

    /**
     * Calls the Promise executor or returns new Promise that resolves data passed to this method.
     * This is useful if the data fetched by the promise were externally modified.
     *
     * @param {ReturnType} data
     * @returns {Promise<ReturnType>}
     */
    public requestOrReturn(data: ReturnType): Promise<ReturnType> {
        if (this.state === PromiseOneState.Resolved) {
            return new Promise<ReturnType>((resolve, reject) => resolve(data));
        } else {
            return this.request();
        }
    }

    /**
     * Calls the Promise executor or returns new Promise that resolves data passed by callback to this method.
     * This is useful if the data fetched by the promise were externally modified.
     *
     * @param {() => ReturnType} callback
     * @returns {Promise<ReturnType>}
     */
    public requestOrReturnCb(callback: () => ReturnType): Promise<ReturnType> {
        if (this.state === PromiseOneState.Resolved) {
            return new Promise<ReturnType>((resolve, reject) => resolve(callback()));
        } else {
            return this.request();
        }
    }

    /**
     * Resets state
     */
    public reset(): void {
        this.state = PromiseOneState.NonRequested;
        this.data = null;
        this.pendingPromises = [];
    }

    /**
     * @returns {boolean}
     */
    public isProcessed(): boolean {
        return this.state === PromiseOneState.Resolved || this.state === PromiseOneState.Rejected;
    }

    /**
     * @returns {boolean}
     */
    public isRejected(): boolean {
        return this.state === PromiseOneState.Rejected;
    }

    /**
     * @returns {boolean}
     */
    public isResolved(): boolean {
        return this.state === PromiseOneState.Resolved;
    }

    /**
     * @returns {boolean}
     */
    public isProcessing(): boolean {
        return this.state === PromiseOneState.Processing;
    }

    /**
     * @returns {boolean}
     */
    public isNonRequested(): boolean {
        return this.state === PromiseOneState.NonRequested;
    }

    /**
     * @returns {PromiseOneState}
     */
    public getState(): PromiseOneState {
        return this.state;
    }
}