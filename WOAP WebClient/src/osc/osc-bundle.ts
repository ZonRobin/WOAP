import * as OSC from "osc-min";
import OSCElement from "./osc-element";
import * as Buffer from "buffer";

export default class OSCBundle extends OSCElement {
    /**
     * @type {string}
     */
    public readonly oscType: string = "bundle";

    /**
     * @type {number}
     */
    public timetag: number;

    /**
     * @type {Array<OSCElement>}
     */
    public elements: Array<OSCElement> = [];

    /**
     * @param {number} timeTag
     * @param {Array<OSCElement>} elements
     */
    constructor(timeTag?: number, elements: Array<OSCElement> = []) {
        super();
        this.timetag = timeTag;
        this.elements = elements;
    }

    /**
     * Creates new OSC bundle
     * Alias for constructor
     *
     * @param {number} timeTag
     * @returns {OSCBundle}
     */
    static create(timeTag?: number) {
        return new OSCBundle(timeTag);
    }

    /**
     * @param {OSCElement} newElement
     * @returns {OSCBundle}
     */
    public addElement(newElement: OSCElement): this {
        this.elements.push(newElement);
        return this;
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
        let output = "Timetag: " + this.timetag;
        for (let element of this.elements) {
            if (element.isMessage()) {
                output += "--- Message" + element.toString();
            }
            else {
                 output += "--- Bundle" + element.toString();
            }
        }

        return output;
    }
}