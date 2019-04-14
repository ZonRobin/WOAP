import OSCMessage from "../../osc/osc-message";
import {ConfigurationManager} from "../../configuration-manager";
import {OSCArgumentType} from "../../osc/osc-argument";

export default abstract class ValidatableOSCMessage {
    /**
     * @type {OSCMessage}
     */
    protected message: OSCMessage;

    /**
     * @param {OSCMessage} message
     * @param {boolean} initializeMessageOnNull
     */
    public constructor(message: OSCMessage = null, initializeMessageOnNull: boolean = true) {
        if (message === null && initializeMessageOnNull) {
            message = new OSCMessage(this.getValidAddress());
        }

        this.message = message;
    }

    /**
     * Extension for constructor
     */
    protected initializeMessage(): void {
        if (this.message === null) {
            this.message = new OSCMessage(this.getValidAddress());
        }
    }

    /**
     * @param {Array<any>} argSet
     * @returns {ValidatableOSCMessage}
     */
    protected addArgumentSet(argSet: Array<any>): this {
        let argumentTypes = this.getValidArgumentTypes();

        if (argSet.length !== argumentTypes.length) {
            throw new Error("Trying to add " + argSet.length + " arguments instead of " + argumentTypes.length);
        }

        for (let i = 0; i < argumentTypes.length; ++i) {
            this.message.addArgument(argumentTypes[i], argSet[i]);
        }

        return this;
    }

    /**
     * @returns {boolean}
     */
    protected validate(): boolean {
        return true;
    }

    /**
     * @returns {string}
     */
    public abstract getValidAddress(): string;

    /**
     * @returns {OSCArgumentType[]}
     */
    public getValidArgumentTypes(): OSCArgumentType[] {
        return [];
    }

    /**
     * @returns {OSCMessage}
     */
    public toOSCMessage(): OSCMessage {
        return this.message;
    }

    /**
     * @returns {boolean}
     */
    public isValid(): boolean {
        if (!this.isAddressValid() || !this.areArgumentsValid() || !this.validate()) {
            this.logMessageInvalidity();
            return false;
        }
        else {
            return true;
        }
    }

    /**
     * @returns {boolean}
     */
    protected isAddressValid(): boolean {
        if (ConfigurationManager.isDevelopment()) {
            return this.getMessageAddress() === this.getValidAddress();
        }

        return true;
    }

    /**
     * @returns {boolean}
     */
    protected areArgumentsValid(): boolean {
        let argumentTypes = this.getValidArgumentTypes();

        if (!this.hasNumArgs(argumentTypes.length)) {
            return false;
        }

        for (let i = 0; i < argumentTypes.length; ++i) {
            if (this.message.args[i].type !== argumentTypes[i]) {
                return false;
            }
        }

        return true;
    }

    /**
     * @returns {string}
     */
    protected getMessageAddress(): string {
        return this.message.address;
    }

    /**
     * @param {number} offset
     * @returns {any}
     */
    protected getArgumentByOffset(offset: number): any {
        return this.message.args[offset].value;
    }

    /**
     * @param {number} count
     * @returns {boolean}
     */
    protected hasNumArgs(count: number): boolean {
        return this.message.args.length === count;
    }

    protected logMessageInvalidity(): void {
        if (ConfigurationManager.isDebug()) {
            console.log("Message is not valid");
            console.log(this.message.toString());

            if (!this.isAddressValid()) {
                console.log("Address is not valid. Got " + this.getMessageAddress() + ", expected " + this.getValidAddress());
            }
            if (!this.areArgumentsValid()) {
                console.log("Arguments are not valid");
            }
            if (!this.validate()) {
                console.log("Custom validation failed");
            }
        }
    }
}
