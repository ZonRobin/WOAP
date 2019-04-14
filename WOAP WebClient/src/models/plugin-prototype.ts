import PluginParameterPrototype from "./plugin-parameter-prototype";
import PluginViewModel from "./plugin-view-model";
import PNGImage from "../components/png-image";

export default class PluginPrototype {
    protected views: PluginViewModel[] = [];

    protected ref: string;

    protected manufacturer: string;

    protected version: string;

    protected id: number;

    protected parameters: PluginParameterPrototype[] = [];

    protected images: PNGImage[] = [];

    protected script: string = "";

    public setRef(newRef: string): this {
        this.ref = newRef;

        return this;
    }

    public getRef(): string {
        return this.ref;
    }

    public setManufacturer(newManufacturer: string): this {
        this.manufacturer = newManufacturer;
        return this;
    }

    public getManufacturer(): string {
        return this.manufacturer;
    }

    public setVersion(newVersion: string): this {
        this.version = newVersion;

        return this;
    }

    public getVersion(): string {
        return this.version;
    }

    public setId(newId: number): this {
        this.id = newId;

        return this;
    }

    public getId(): number {
        return this.id;
    }

    public addParameter(parameterToAdd: PluginParameterPrototype): this {
        this.parameters.push(parameterToAdd);

        return this;
    }

    public addView(view: PluginViewModel): this {
        this.views.push(view);
        return this;
    }

    public getViews(): PluginViewModel[] {
        return this.views;
    }

    public getParameterByRef(ref: string): PluginParameterPrototype {
        for (let parameter of this.parameters) {
            if (parameter.getRef() === ref) {
                return parameter;
            }
        }

        return null;
    }

    public getParameters(): Array<PluginParameterPrototype> {
        return this.parameters;
    }

    public getNumParameters(): number {
        return this.parameters.length;
    }

    public addImage(image: PNGImage): this {
        this.images.push(image);
        return this;
    }

    public getImages(): PNGImage[] {
        return this.images;
    }

    public setScript(newScript: string, type: string = "text/javascript"): this {
        switch (type) {
            case "text/javascript": {
                return this.setScriptJavaScript(newScript);
            }
            default: {
                throw new Error("Unknown script type (" + type + ")");
            }
        }
    }

    public setScriptJavaScript(newScript: string): this {
        this.script = newScript;

        return this;
    }

    public getScript(): string {
        return this.script;
    }
}