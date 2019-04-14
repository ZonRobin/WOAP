import {PluginParameterValueType} from "./plugin-parameter-model";
import {ArrayExtended} from "../array-extended";

export default class PluginParameterPrototype {
    protected valueType: PluginParameterValueType;

    protected index: number;

    protected ref: string;

    protected name: string = "";

    protected steps: ArrayExtended<number> = new ArrayExtended<number>();

    constructor(valueType: PluginParameterValueType, index: number, ref: string) {
        this.valueType = valueType;
        this.index = index;
        this.ref = ref;
    }

    public setName(newName: string): this {
        this.name = newName;

        return this;
    }

    public getRef(): string {
        return this.ref;
    }

    public getIndex(): number {
        return this.index;
    }

    public static isValidType(type: string): boolean {
        for (let i in PluginParameterValueType) {
            if (PluginParameterValueType[i] === type) {
                return true;
            }
        }

        return false;
    }

    public getName(): string {
        return this.name;
    }

    public getValueType(): PluginParameterValueType {
        return this.valueType;
    }

    public setSteps(newSteps: Array<number>): this {
        this.steps.setArray(newSteps);
        return this;
    }

    public addStep(newStep: number): this {
        this.steps.addUnique(newStep);
        return this;
    }

    public getNumSteps(): number {
        return this.steps.getLength();
    }

    public getSteps(): Array<number> {
        return this.steps.toArray();
    }

    public isSwitchType(): boolean {
        return this.valueType === PluginParameterValueType.Switch;
    }

    public isFloatType(): boolean {
        return this.valueType === PluginParameterValueType.Float;
    }
}