import {Utility} from "../utility";
import PluginPrototype from "../models/plugin-prototype";
import PluginParameterPrototype from "../models/plugin-parameter-prototype";
import {PluginConfigParser} from "../stores/plugin-store";
import PluginViewPrototype from "../models/plugin-view-model";
import {PluginParameterValueType} from "../models/plugin-parameter-model";
import PNGImage from "../components/png-image";

export default class PAGConfigParserXML implements PluginConfigParser {
    protected xmlParser: DOMParser;

    public readonly supportedConfigVersions: string[];

    constructor() {
        this.xmlParser = new DOMParser();
        this.supportedConfigVersions = ["1.0"];
    }

    public parseFile(path: string): Promise<PluginPrototype> {
        return new Promise<PluginPrototype>((resolve: (prototype: PluginPrototype) => void, reject: (reason: PAGParsingFailReason) => void) => {
            let xmlhttp = new XMLHttpRequest();
            xmlhttp.open("GET", path, true);
            xmlhttp.onreadystatechange = () => {
                if (xmlhttp.readyState === XMLHttpRequest.DONE) {
                    try {
                        if (xmlhttp.status === 404) {
                            reject(PAGParsingFailReason.FileNotFound);
                        } else if (xmlhttp.status !== 200) {
                            reject(PAGParsingFailReason.UnknownError);
                        } else {
                            let pluginPrototype = this.parse(xmlhttp.responseText);
                            resolve(pluginPrototype);
                        }
                    } catch (error) {
                        reject(error.message);
                    }
                }
            };

            xmlhttp.setRequestHeader("Accept", "text/xml");
            xmlhttp.responseType = "text";
            xmlhttp.send(null);
        });
    }

    public parse(xmlString: string): PluginPrototype {
        let xmlDoc;

        try {
            xmlDoc = Utility.parseXML(xmlString);
        } catch (error) {
            throw new SyntaxError("Plugin config can\"t be parsed: " + error.message);
        }

        let rootElements = xmlDoc.getElementsByTagName("PAGConfig");
        let version = "";

        if (rootElements.length < 1) {
            throw new SyntaxError("Root element (PAGConfig) not found");
        } else if (rootElements.length > 1) {
            throw new SyntaxError("There\"s more than one root element (PAGConfig) found");
        }

        let rootElement = rootElements[0];
        if (rootElement.hasAttribute("version")) {
            version = rootElement.getAttribute("version");
        }

        if (this.supportedConfigVersions.indexOf(version) === -1) {
            throw new UnsupportedVersionError("Version " + String(version) + " is not supported.");
        }

        for (let i = 0; i < rootElement.children.length; ++i) {
            if (rootElement.children[i].tagName.toLowerCase() === "plugin") {
                let plugin = this.parseSinglePlugin(rootElement.children[i]);
                if (plugin !== null) {
                    return plugin;
                }
            }
        }

        return null;
    }

    public parseSinglePlugin(pluginElement: Element): PluginPrototype {
        let pluginPrototype = new PluginPrototype();

        if (pluginElement.hasAttribute("ref")) {
            pluginPrototype.setRef(pluginElement.getAttribute("ref"));
        }

        if (pluginElement.hasAttribute("manufacturer")) {
            pluginPrototype.setManufacturer(pluginElement.getAttribute("manufacturer"));
        }

        if (pluginElement.hasAttribute("version")) {
            pluginPrototype.setVersion(pluginElement.getAttribute("version"));
        }

        if (pluginElement.hasAttribute("id")) {
            pluginPrototype.setId(parseInt(pluginElement.getAttribute("id"), 10));
        }

        let childElements = pluginElement.children;
        for (let i = 0; i < childElements.length; ++i) {
            let child = childElements[i];

            switch (child.tagName.toLowerCase()) {
                case "parameters": {
                    for (let parameter of this.parseParameters(child)) {
                        pluginPrototype.addParameter(parameter);
                    }
                    break;
                }
                case "views": {
                    for (let view of this.parseViews(child)) {
                        pluginPrototype.addView(view);
                    }
                    break;
                }
                case "script": {
                    if (!child.hasAttribute("type")) {
                        throw new Error("Type attribute not found on script");
                    }

                    pluginPrototype.setScript(Utility.trimString(child.textContent), child.getAttribute("type").toLowerCase());
                    break;
                }
                case "images": {
                    for (let image of this.parseImages(child)) {
                        pluginPrototype.addImage(image);
                    }
                    break;
                }
                default:
                    break;
            }
        }

        return pluginPrototype;
    }

    public parseViews(viewsElement: Element): PluginViewPrototype[] {
        let output = [];
        let children = viewsElement.children;

        for (let i = 0; i < children.length; ++i) {
            let child = children[i];
            if (child.tagName.toLowerCase() === "view") {
                output.push(this.parseSingleView(child));
            }
        }

        return output;
    }

    public parseParameters(parametersElement: Element): PluginParameterPrototype[] {
        let output = [];
        let children = parametersElement.getElementsByTagName("parameter");

        for (let i = 0; i < children.length; ++i) {
            output.push(this.parseSingleParameter(children[i], i));
        }

        return output;
    }

    public parseSingleView(viewElement: Element): PluginViewPrototype {
        let pluginViewPrototype = new PluginViewPrototype();

        if (!viewElement.hasAttribute("lang")) {
            throw new Error("Lang attribute not found on layout");
        }

        pluginViewPrototype.setLayout(viewElement.cloneNode(true) as Element, viewElement.getAttribute("lang").toLowerCase());
        pluginViewPrototype.setIsDefault(Utility.xNull(viewElement.getAttribute("default"), false));

        if (viewElement.hasAttribute("name")) {
            pluginViewPrototype.setName(viewElement.getAttribute("name"));
        }

        return pluginViewPrototype;
    }

    public parseSingleParameter(parameterElement: Element, index: number): PluginParameterPrototype {
        let ref = parameterElement.getAttribute("ref");
        let valueType = Utility.xNull(parameterElement.getAttribute("type"), PluginParameterValueType.Float);

        if (ref.length <= 0) {
            throw new Error("Parameter ref attribute not found");
        }

        if (!PluginParameterPrototype.isValidType(valueType)) {
            throw new Error("Wrong valueType parameter");
        }

        let parameterPrototype = new PluginParameterPrototype(valueType as PluginParameterValueType, index, ref);

        let parameterName = parameterElement.getAttribute("name");
        if (null !== parameterName) {
            parameterPrototype.setName(parameterName);
        }

        if (parameterPrototype.isSwitchType()) {
            if (parameterElement.children.length < 2) {
                parameterPrototype.setSteps([0, 1]);
            } else {
                let stepElements = parameterElement.children;
                for (let i = 0; i < stepElements.length; ++i)
                {
                    if (!stepElements[i].hasAttribute("value")) {
                        throw new Error("Step value for parameter " + parameterPrototype.getName() + " is missing.");
                    }

                    let stepValue = Utility.strToFloat(stepElements[i].getAttribute("value"), -1);
                    if (stepValue < 0 || stepValue > 1) {
                        throw new Error("Step value for parameter " + parameterPrototype.getName() + " is out of range.");
                    }

                    parameterPrototype.addStep(stepValue);
                }
            }
        }

        return parameterPrototype;
    }

    public parseImages(imagesElement: Element): PNGImage[] {
        let output = [];
        let children = imagesElement.getElementsByTagName("image");

        for (let i = 0; i < children.length; ++i) {
            output.push(this.parseSingleImage(children[i], i));
        }

        return output;
    }

    public parseSingleImage(imageElement: Element, index: number): PNGImage {
        let ref = imageElement.getAttribute("ref");
        if (ref.length <= 0) {
            throw new Error("Image ref not found");
        }

        let data = imageElement.textContent.replace(/\s+/, "");
        return new PNGImage(ref, data);
    }
}

class PAGParseError extends Error {
    public reason: PAGParsingFailReason;

    constructor(message: string, reason: PAGParsingFailReason) {
        super(message);
        this.reason = reason;
    }
}

export enum PAGParsingFailReason {
    FileParsingError = "file parsing error",
    FileNotFound = "file not found",
    RootElementNotFound = "root element not found",
    MultipleRootElements = "multiple root elements",
    UnknownError = "unknown error"
}

class UnsupportedVersionError extends Error {
    constructor(message: string) {
        super(message);
        Object.setPrototypeOf(this, UnsupportedVersionError.prototype);
    }
}