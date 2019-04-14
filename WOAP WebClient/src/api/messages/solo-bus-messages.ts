import ValidatableOSCMessage from "./validatable-osc-message";
import {OSCArgumentType} from "../../osc/osc-argument";

export class SoloBusAddAudioDataListenerMessage extends ValidatableOSCMessage {
    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [
            OSCArgumentType.Integer
        ];
    }

    /**
     * @param {number} minNumSamples
     * @returns {this}
     */
    public addArguments(minNumSamples: number): this {
        return this.addArgumentSet([minNumSamples]);
    }

    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return SoloBusAddAudioDataListenerMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/soloBus/addAudioDataListener";
    }

    /**
     * @returns {number}
     */
    public getMinNumSamplesInMessage(): number {
        return this.getArgumentByOffset(0);
    }
}

export class SoloBusRemoveAudioDataListenerMessage extends ValidatableOSCMessage {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return SoloBusRemoveAudioDataListenerMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return "/soloBus/removeAudioDataListener";
    }
}