import ValidatableOSCMessage from "./validatable-osc-message";


export class ApiCoreErrorMessage extends ValidatableOSCMessage {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return ApiCoreErrorMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return '/error';
    }
}

export class ApiCoreReadyMessage extends ValidatableOSCMessage {
    /**
     * @returns {string}
     */
    public getValidAddress(): string {
        return ApiCoreReadyMessage.getAddress();
    }

    /**
     * @returns {string}
     */
    public static getAddress(): string {
        return '/ready';
    }
}