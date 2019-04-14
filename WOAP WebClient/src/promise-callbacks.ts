
export default class PromiseCallbacks<T> {
    public resolve: (value: T) => void;

    public reject: (reason?: any) => void;

    constructor(resolve: (value: T) => void, reject: (reason?: any) => void) {
        this.resolve = resolve;
        this.reject = reject;
    }
}