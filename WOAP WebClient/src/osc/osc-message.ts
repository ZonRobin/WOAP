import * as OSC from "osc-min";
import OSCElement from "./osc-element";
import {OSCArgument, OSCArgumentType} from "./osc-argument";
import * as Buffer from "buffer";

export default class OSCMessage extends OSCElement {
    /**
     * @type {string}
     */
    public readonly oscType: string = "message";

    /**
     * @type {string}
     */
    public address: string;

    /**
     * @type {Array<OSCArgument>}
     */
    public args: Array<OSCArgument> = [];

    /**
     * @param {string} address
     * @param {Array<OSCArgument>} args
     */
    constructor(address?: string, args: Array<OSCArgument> = []) {
        super();
        this.address = address;
        this.args = args;
    }

    /**
     * Create new OSC message
     * Alias for constructor
     *
     * @param {string} address
     * @returns {OSCMessage}
     */
    static create(address: string) {
        return new OSCMessage(address);
    }

    /**
     * @param {OSCArgumentType} type
     * @param {any} value
     * @returns {OSCMessage}
     */
    public addArgument(type: OSCArgumentType, value?: any): this {
        this.args.push(new OSCArgument(type, value));
        return this;
    }

    /**
     * @param {string} value
     * @returns {OSCMessage}
     */
    public addArgumentString(value: string): this {
        this.args.push(new OSCArgument(OSCArgumentType.String, value));
        return this;
    }

    /**
     * @param {number} value
     * @returns {OSCMessage}
     */
    public addArgumentFloat(value: number): this {
        this.args.push(new OSCArgument(OSCArgumentType.Float, value));
        return this;
    }

    /**
     * @param {number} value
     * @returns {OSCMessage}
     */
    public addArgumentInteger(value: number): this {
        this.args.push(new OSCArgument(OSCArgumentType.Integer, value));
        return this;
    }

    /**
     * @param {Buffer} value
     * @returns {OSCMessage}
     */
    public addArgumentBlob(value: Buffer): this {
        this.args.push(new OSCArgument(OSCArgumentType.Blob, value));
        return this;
    }

    /**
     * @returns {OSCMessage}
     */
    public addArgumentTrue(): this {
        this.args.push(new OSCArgument(OSCArgumentType.True));
        return this;
    }

    /**
     * @returns {OSCMessage}
     */
    public addArgumentFalse(): this {
        this.args.push(new OSCArgument(OSCArgumentType.False));
        return this;
    }

    /**
     * @returns {OSCMessage}
     */
    public addArgumentNull(): this {
        this.args.push(new OSCArgument(OSCArgumentType.Null));
        return this;
    }

    /**
     * @param {string} value
     * @returns {OSCMessage}
     */
    public addArgumentTimeTag(value: string): this {
        this.args.push(new OSCArgument(OSCArgumentType.TimeTag, value));
        return this;
    }

    /**
     * @param {Array<OSCArgument>} value
     * @returns {OSCMessage}
     */
    public addArgumentArray(value: Array<OSCArgument>): this {
        this.args.push(new OSCArgument(OSCArgumentType.Array, value));
        return this;
    }

    /**
     * @param {number} position
     * @param {OSCArgumentType} type
     * @param {any} value
     * @returns {OSCMessage}
     * @throws {Error}
     */
    public replaceArgument(position: number, type: OSCArgumentType, value?: any): this {
        if (this.args.length <= position) {
            this.args[position] = new OSCArgument(type, value);
        } else {
            throw Error("Can\"t replace " + position + " argument. The message has only " + this.args.length + " arguments");
        }

        return this;
    }

    /**
     * @param {number} position
     * @param {string} value
     * @returns {OSCMessage}
     * @throws {Error}
     */
    public replaceArgumentByString(position: number, value: string): this {
        return this.replaceArgument(position, OSCArgumentType.String, value);
    }

    /**
     * @param {number} position
     * @param {number} value
     * @returns {OSCMessage}
     * @throws {Error}
     */
    public replaceArgumentByFloat(position: number, value: number): this {
        return this.replaceArgument(position, OSCArgumentType.Float, value);
    }

    /**
     * @param {number} position
     * @param {number} value
     * @returns {OSCMessage}
     * @throws {Error}
     */
    public replaceArgumentByInteger(position: number, value: number): this {
        return this.replaceArgument(position, OSCArgumentType.Integer, value);
    }

    /**
     * @param {number} position
     * @param {Buffer} value
     * @returns {OSCMessage}
     * @throws {Error}
     */
    public replaceArgumentByBlob(position: number, value: Buffer): this {
        return this.replaceArgument(position, OSCArgumentType.Blob, value);
    }

    /**
     * @param {number} position
     * @returns {OSCMessage}
     * @throws {Error}
     */
    public replaceArgumentByTrue(position: number): this {
        return this.replaceArgument(position, OSCArgumentType.True);
    }

    /**
     * @param {number} position
     * @returns {OSCMessage}
     * @throws {Error}
     */
    public replaceArgumentByFalse(position: number): this {
        return this.replaceArgument(position, OSCArgumentType.False);
    }

    /**
     * @param {number} position
     * @returns {OSCMessage}
     * @throws {Error}
     */
    public replaceArgumentByNull(position: number): this {
        return this.replaceArgument(position, OSCArgumentType.Null);
    }

    /**
     * @param {number} position
     * @param {string} value
     * @returns {OSCMessage}
     * @throws {Error}
     */
    public replaceArgumentByTimeTag(position: number, value: string): this {
        return this.replaceArgument(position, OSCArgumentType.TimeTag, value);
    }

    /**
     * @param {number} position
     * @param {Array<OSCArgument>} value
     * @returns {OSCMessage}
     * @throws {Error}
     */
    public replaceArgumentByArray(position: number, value: Array<OSCArgument>): this {
        return this.replaceArgument(position, OSCArgumentType.Array, value);
    }

    /**
     * @returns {Buffer}
     */
    public toBuffer(): Buffer {
        return OSC.toBuffer(this);
    }

    /**
     * To string
     */
    public toString(): string {
        let output = "Address: " + this.address + "\n";

        if (this.args.length === 0) {
            return output;
        }

        output += "Args: ";
        let argStrings = [];

        for (let argument of this.args) {
            argStrings.push(argument.toString());
        }

        return output + argStrings.join(" | ");
    }
}