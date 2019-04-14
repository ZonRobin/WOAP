export default class PluginViewModel {
    protected layout: Element;

    protected isDefault: boolean = false;

    protected name: string;

    public setLayout(newLayout, lang: string = "pag-jsx"): this {
        switch (lang) {
            case "pag-jsx": {
                return this.setLayoutJSX(newLayout);
            }
            default: {
                return this;
            }
        }
    }

    public setLayoutJSX(newLayout: Element): this {
        this.layout = newLayout;

        return this;
    }

    public setIsDefault(newIsDefault: boolean): this {
        this.isDefault = newIsDefault;

        return this;
    }

    public getIsDefault(): boolean {
        return this.isDefault;
    }

    public setName(newName: string): this {
        this.name = newName;

        return this;
    }

    public getLayout(): Element {
        return this.layout;
    }
}