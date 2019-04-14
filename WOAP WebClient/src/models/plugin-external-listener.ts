import TrackPluginModel from "./track-plugin-model";
import PluginParameterModel from "./plugin-parameter-model";

export interface PluginExternalListenerInterface {
    viewCreated: () => void;
    viewWillBeDeleted: () => void;
    parameterChanged: (parameter: PluginParameterModel, newValue: number, newValueText: string) => void;
    activeChanged: (newState: boolean) => void;
    bypassChanged: (newState: boolean) => void;
    nameChanged: (newName: string) => void;
}

export class PluginExternalListener implements PluginExternalListenerInterface {
    protected pluginModel: TrackPluginModel;

    constructor(pluginModel: TrackPluginModel) {
        this.pluginModel = pluginModel;
    }

    public static readonly overridableFunctions: Array<string> = [
        "viewCreated",
        "viewWillBeDeleted",
        "parameterChanged",
        "activeChanged",
        "bypassChanged",
        "nameChanged"
    ];

    public viewCreated(): void {
    }

    public viewWillBeDeleted(): void {
    }

    public parameterChanged(parameterModel: PluginParameterModel, newValue: number, newValueText: string): void {
    }

    public activeChanged(newState: boolean): void {
    }

    public bypassChanged(newState: boolean): void {
    }

    public nameChanged(newName: string): void {
    }
}