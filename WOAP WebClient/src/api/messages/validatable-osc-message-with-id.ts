import OSCMessage from "../../osc/osc-message";
import {ConfigurationManager} from "../../configuration-manager";
import {OSCArgumentType} from "../../osc/osc-argument";
import ValidatableOSCMessage from "./validatable-osc-message";

export default abstract class ValidatableOSCMessageWithId extends ValidatableOSCMessage {
    protected static nextMessageId = 1;

    /**
     * @param {OSCMessage} message
     * @param {boolean} initializeMessageOnNull
     */
    public constructor(message: OSCMessage = null, initializeMessageOnNull: boolean = true) {
        super(message, initializeMessageOnNull);

        if (message === null && initializeMessageOnNull) {
            /* message got initialized by super constructor */
            this.message.addArgumentInteger(ValidatableOSCMessageWithId.getNextMessageId());
        }
    }

    /**
     * @param {number} newValue
     * @returns {ValidatableOSCMessageWithId}
     */
    public setMessageId(newValue: number): this {
        if (ConfigurationManager.isDevelopment()) {
            if (newValue === 0) {
                throw new Error("You should not set ID of message to 0");
            } else if (this.getArgumentByOffset(0) !== 0) {
                throw new Error("You should never change ID of message");
            }
        }

        this.message.replaceArgumentByInteger(0, newValue);
        return this;
    }

    /**
     * @returns {boolean}
     */
    public isMessageWithId(): boolean {
        return this.message.args.length > 0 && this.message.args[0].isInteger();
    }

    /**
     * @param {OSCMessage} message
     * @returns {boolean}
     */
    public static doesMessageHaveId(message: OSCMessage): boolean {
        return message.args.length > 0 && message.args[0].isInteger();
    }

    /**
     * Call doesMessageHaveId first
     *
     * @param {OSCMessage} message
     * @returns {number}
     */
    public static getMessageId(message: OSCMessage): number {
        return message.args[0].value;
    }

    /**
     * @returns {number}
     */
    public getMessageId(): number {
        return this.message.args[0].value;
    }

    /**
     * @returns {boolean}
     */
    public isValid(): boolean {
        if (!this.isMessageWithId() || !this.isAddressValid() || !this.areArgumentsValid() || !this.validate()) {
            this.logMessageInvalidity();
            return false;
        }
        else {
            return true;
        }
    }

    /**
     * Extension for constructor
     */
    protected initializeMessage(): void {
        if (this.message === null) {
            this.message = new OSCMessage(this.getValidAddress());
            this.message.addArgumentInteger(ValidatableOSCMessageWithId.getNextMessageId());
        }
    }

    /**
     * @returns {boolean}
     */
    protected areArgumentsValid(): boolean {
        let argumentTypes = this.getValidArgumentTypes();

        if (!this.hasNumArgs(argumentTypes.length)) {
            return false;
        }

        if (this.message.args[0].type !== OSCArgumentType.Integer) {
            return false;
        }

        for (let i = 0; i < argumentTypes.length; ++i) {
            if (this.message.args[i + 1].type !== argumentTypes[i]) {
                return false;
            }
        }

        return true;
    }

    /**
     * @param {number} offset
     * @returns {any}
     */
    protected getArgumentByOffset(offset: number): any {
        return this.message.args[offset + 1].value;
    }

    /**
     * @param {number} count
     * @returns {boolean}
     */
    protected hasNumArgs(count: number): boolean {
        return (this.message.args.length - 1) === count;
    }

    /**
     * @returns {number}
     */
    protected static getNextMessageId() {
        return ++ValidatableOSCMessageWithId.nextMessageId;
    }
}
