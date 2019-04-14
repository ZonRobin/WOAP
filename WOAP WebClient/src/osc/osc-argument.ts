export enum OSCArgumentType
{
    String = "string",
    Float = "float",
    Integer = "integer",
    Blob = "blob",
    True = "true",
    False = "false",
    Null = "null",
    Bang = "bang",
    TimeTag = "timetag",
    Array = "array"
}

export class OSCArgument {
    /**
     * @type {OSCArgumentType}
     */
    public type: OSCArgumentType = OSCArgumentType.Null;

    /**
     * @type {any}
     */
    public value: any;

    /**
     * @param {OSCArgumentType} type
     * @param {any} value
     */
    constructor(type: OSCArgumentType, value?: any) {
        this.type = type;
        this.value = value;
    }

    /**
     * @returns {boolean}
     */
    public isString(): boolean {
        return this.type === OSCArgumentType.String;
    }

    /**
     * @returns {boolean}
     */
    public isFloat(): boolean {
        return this.type === OSCArgumentType.Float;
    }

    /**
     * @returns {boolean}
     */
    public isInteger(): boolean {
        return this.type === OSCArgumentType.Integer;
    }

    /**
     * @returns {boolean}
     */
    public isBlob(): boolean {
        return this.type === OSCArgumentType.Blob;
    }

    /**
     * @returns {boolean}
     */
    public isTrue(): boolean {
        return this.type === OSCArgumentType.True;
    }

    /**
     * @returns {boolean}
     */
    public isFalse(): boolean {
        return this.type === OSCArgumentType.False;
    }

    /**
     * @returns {boolean}
     */
    public isNull(): boolean {
        return this.type === OSCArgumentType.Null;
    }

    /**
     * @returns {boolean}
     */
    public isBang(): boolean {
        return this.type === OSCArgumentType.Bang;
    }

    /**
     * @returns {boolean}
     */
    public isTimeTag(): boolean {
        return this.type === OSCArgumentType.TimeTag;
    }

    /**
     * @returns {boolean}
     */
    public isArray(): boolean {
        return this.type === OSCArgumentType.Array;
    }

    /**
     * @returns {string}
     */
    public toString(): string {
        switch (this.type) {
            case OSCArgumentType.String:
            case OSCArgumentType.Float:
            case OSCArgumentType.Integer:
                return this.value.toString();
            case OSCArgumentType.Blob:
                return "blob(" + this.value.length + ")";
            case OSCArgumentType.True:
                return "true";
            case OSCArgumentType.False:
                return "false";
            case OSCArgumentType.Null:
                return "null";
            case OSCArgumentType.Bang:
                return "bang";
            case OSCArgumentType.TimeTag:
                return "TimeTag: " + this.value;
            case OSCArgumentType.Array: {
                let parts = [];
                for (let arg of this.value) {
                    parts.push(arg.toString());
                }

                return "[" + parts.join(", ") + "]";
            }
            default:
                return "";
        }
    }
}