import * as OSC from "osc-min";
import OSCMessage from "./osc-message";
import OSCBundle from "./osc-bundle";
import OSCElement from "./osc-element";
import {OSCArgument, OSCArgumentType} from "./osc-argument";
import * as Buffer from "buffer";

export default class OSCMapper {
    /**
     * @param {Buffer} data
     * @returns {OSCElement}
     */
    public static fromBuffer(data: Buffer): OSCElement {
        let element = OSC.fromBuffer(data, true);

        return OSCMapper.toElement(element);
    }

    /**
     * Map osc-min data object to OSCElement
     *
     * @param data
     * @returns {OSCElement}
     */
    public static toElement(data: any): OSCElement {
        if (data.oscType === "message") {
            return OSCMapper.toMessage(data);
        }
        else if (data.oscType === "bundle") {
            return OSCMapper.toBundle(data);
        }
        else {
            throw new Error("Unknown OSC type of message");
        }
    }

    /**
     * Map osc-min data object to OSCMessage
     *
     * @param data
     * @returns {OSCMessage}
     */
    public static toMessage(data: any): OSCMessage {
        let outArgs: Array<OSCArgument> = [];

        for (let argument of data.args) {
            outArgs.push(new OSCArgument(argument.type as OSCArgumentType, argument.value));
        }

        return new OSCMessage(data.address, outArgs);
    }

    /**
     * Map osc-min data object to OSCBundle
     *
     * @param data
     * @returns {OSCBundle}
     */
    public static toBundle(data: any): OSCBundle {
        let outElements: Array<OSCElement> = [];

        for (let element of data.elements) {
            outElements.push(OSCMapper.toElement(element));
        }

        return new OSCBundle(data.timetag, outElements);
    }
}
