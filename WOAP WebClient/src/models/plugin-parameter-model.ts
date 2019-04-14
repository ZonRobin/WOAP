import {Removable} from "../removable";
import {Comparable} from "../comparable";
import {APIConsumer} from "../api/api-consumer";
import {Utility, UUID} from "../utility";
import {ArrayExtended} from "../array-extended";
import {PluginSetParameterValueMessage} from "../api/messages/plugin-messages";

export interface TrackPluginParameterModelListener {
    valueChanged(newValue: number, newValueText: string): void;
}

export default class PluginParameterModel extends APIConsumer implements Comparable, Removable {
    protected pluginId: UUID;
    protected index: number;
    protected ref: string;
    protected value: number = 0;
    protected valueText: string = "0";
    protected valueType: PluginParameterValueType = PluginParameterValueType.Float;
    protected name: string = "";
    protected steps: ArrayExtended<number> = new ArrayExtended<number>();

    protected listeners: ArrayExtended<TrackPluginParameterModelListener> = new ArrayExtended();

    public static readonly MessageType = {
        ValueChanged: 0
    };

    constructor(pluginId: UUID, index: number, ref: string) {
        super();
        this.pluginId = pluginId;
        this.index = index;
        this.ref = ref;
        this.name = ref;

        this.initializeAPIConsumer();
    }

    public setValueAndText(newValue: number, newValueText: string): this {
        this.value = newValue;
        if (newValueText.length === 0) {
            this.valueText = Utility.round(newValue * 100, 2).toString();
        } else {
            let newValueTextNumber = parseFloat(newValueText);
            if (isNaN(newValueTextNumber)) {
                this.valueText = newValueText;
            } else {
                this.valueText = Utility.round(newValueTextNumber, 4).toString();
            }
        }

        this.listeners.forEach((listener) => listener.valueChanged(this.value, this.valueText));
        return this;
    }

    public setValue(newValue: number): this {
        if (newValue !== this.value) {
            this.value = newValue;
            this.sendAPIMessage(new PluginSetParameterValueMessage(this.pluginId).addArguments(this.index, newValue));
        }

        return this;
    }

    public getIndex(): number {
        return this.index;
    }

    public getRef(): string {
        return this.ref;
    }

    public getValue(): number {
        return this.value;
    }

    public getValueText(): string {
        return this.valueText;
    }

    public getValueType(): PluginParameterValueType {
        return this.valueType;
    }

    public isSwitchType(): boolean {
        return this.valueType === PluginParameterValueType.Switch;
    }

    public isFloatType(): boolean {
        return this.valueType === PluginParameterValueType.Float;
    }

    public getName(): string {
        return this.name;
    }

    public setName(newName: string): this {
        this.name = newName;
        return this;
    }

    public setValueType(newType: PluginParameterValueType): this {
        this.valueType = newType;
        return this;
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

    public getSteps(): ArrayExtended<number> {
        return this.steps;
    }

    public addListener(listener: TrackPluginParameterModelListener): this {
        this.listeners.add(listener);
        return this;
    }

    public removeListener(listener: TrackPluginParameterModelListener): this {
        this.listeners.remove(listener);
        return this;
    }

    public equals(other: Comparable): boolean {
        if (other instanceof PluginParameterModel) {
            return other.index === this.index && other.ref === this.ref;
        }

        return false;
    }

    public instanceWillBeDeleted(): void {
        this.terminateAPIConsumer();
    }
}

export enum PluginParameterValueType {
    Switch = "switch",
    Float = "float"
}