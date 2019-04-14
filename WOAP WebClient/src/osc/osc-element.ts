export default abstract class OSCElement {
    /**
     * @type {string}
     */
    public readonly oscType: string;

    /**
     * @returns {boolean}
     */
    public isMessage(): boolean {
        return this.oscType === "message";
    }

    /**
     * @returns {boolean}
     */
    public isBundle(): boolean {
        return this.oscType === "bundle";
    }

    /**
     * @returns {string}
     */
    public abstract toString(): string;
}