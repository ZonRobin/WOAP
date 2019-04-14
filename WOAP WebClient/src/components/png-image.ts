export default class PNGImage {
    protected ref: string;

    protected data: string;

    constructor(ref: string, data: string) {
        this.ref = ref;
        this.data = data;
    }

    public setRef(newRef: string): this {
        this.ref = newRef;
        return this;
    }

    public getRef(): string {
        return this.ref;
    }

    public getData(): string {
        return this.data;
    }

    public getHTMLData(): string {
        return "data:image/png;base64," + this.data;
    }
}