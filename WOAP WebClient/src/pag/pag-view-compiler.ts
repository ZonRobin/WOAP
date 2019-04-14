import * as HtmlToReact from "html-to-react";
import * as React from "react";
import PAGParameterView from "./components/pag-parameter-view";
import PAGParameterName from "./components/pag-parameter-name";
import PAGParameterValue from "./components/pag-parameter-value";
import PAGPluginName from "./components/pag-plugin-name";
import PAGPluginId from "./components/pag-plugin-id";
import TrackPluginModel from "../models/track-plugin-model";
import PluginViewModel from "../models/plugin-view-model";
import {Utility} from "../utility";
import PAGLinearSlider from "./components/pag-linear-slider";
import PAGRotarySlider from "./components/pag-rotary-slider";
import PAGButton from "./components/pag-button";
import ControlComponentColorScheme from "./control-components/pag-control-component";
import PAGImage from "./components/pag-image";

export default class PAGViewCompiler {
    public getColorSchemeOfNode(node: any) {
        let colorSchemeName = node.attribs["color-scheme"];

        if (Utility.isDefined(colorSchemeName) && colorSchemeName.length > 0) {
            return ControlComponentColorScheme.findByName(colorSchemeName);
        } else {
            if (Utility.isUndefined(node.attribs["color-accent"]) || Utility.isUndefined(node.attribs["color-fill"])) {
                return ControlComponentColorScheme.getDefault();
            } else {
                return new ControlComponentColorScheme(node.attribs["color-accent"], node.attribs["color-fill"]);
            }
        }
    }

    public compile(viewModel: PluginViewModel, pluginModel: TrackPluginModel): React.ReactElement<any> {
        let htmlToReactParser = new HtmlToReact.Parser({xmlMode: true});
        let processNodeDefinitions = new HtmlToReact.ProcessNodeDefinitions(React);

        let processingInstructions = [
            {
                shouldProcessNode: (node) => {
                    return node.name === "PAGParameter";
                },
                processNode: (node, children, index) => {
                    let parameter = pluginModel.getParameterByRef(node.attribs.ref);
                    if (parameter === null) {
                        throw new Error("Can\"t find parameter with ref " + node.attribs.ref);
                    }

                    return React.createElement(PAGParameterView, {
                            parameterModel: parameter,
                            key: index
                        },
                        children);
                }
            }, {
                shouldProcessNode: (node) => {
                    return node.name === "PAGLinearSlider";
                },
                processNode: (node, children, index) => {
                    return React.createElement(PAGLinearSlider, {
                        key: index,
                        width: Utility.strToInt(node.attribs.width, undefined),
                        height: Utility.strToInt(node.attribs.height, undefined),
                        colorScheme: this.getColorSchemeOfNode(node),
                        inverted: Utility.strToBool(node.attribs.inverted),
                        valueBar: Utility.strToBool(Utility.xDef(node.attribs["value-bar"], "1"))
                    });
                }
            }, {
                shouldProcessNode: (node) => {
                    return node.name === "PAGRotarySlider";
                },
                processNode: (node, children, index) => {
                    return React.createElement(PAGRotarySlider, {
                        key: index,
                        size: Utility.strToInt(node.attribs.size, undefined),
                        colorScheme: this.getColorSchemeOfNode(node),
                        inverted: Utility.strToBool(node.attribs.inverted)
                    });
                }
            }, {
                shouldProcessNode: (node) => {
                    return node.name === "PAGButton";
                },
                processNode: (node, children, index) => {
                    return React.createElement(PAGButton, {
                        key: index,
                        width: Utility.strToInt(node.attribs.width, undefined),
                        height: Utility.strToInt(node.attribs.height, undefined),
                        text: node.attribs.text,
                        colorScheme: this.getColorSchemeOfNode(node),
                        inverted: Utility.strToBool(node.attribs.inverted)
                    });
                }
            }, {
                shouldProcessNode: (node) => {
                    return node.name === "PAGImage";
                },
                processNode: (node, children, index) => {
                    let image = pluginModel.getImageByRef(node.attribs.ref);
                    if (image === null) {
                        throw new Error("Can\"t find image with ref " + node.attribs.ref);
                    }

                    return React.createElement(PAGImage, {
                        key: index,
                        image: image
                    });
                }
            }, {
                shouldProcessNode: (node) => {
                    return node.name === "PAGParameterName";
                },
                processNode: (node, children, index) => {
                    return React.createElement(PAGParameterName, {key: index});
                }
            }, {
                shouldProcessNode: (node) => {
                    return node.name === "PAGParameterValue";
                },
                processNode: (node, children, index) => {
                    return React.createElement(PAGParameterValue, {key: index});
                }
            }, {
                shouldProcessNode: (node) => {
                    return node.name === "PAGPluginName";
                },
                processNode: (node, children) => {
                    return React.createElement(PAGPluginName);
                }
            }, {
                shouldProcessNode: (node) => {
                    return node.name === "PAGPluginId";
                },
                processNode: (node, children) => {
                    return React.createElement(PAGPluginId);
                }
            }, {
                shouldProcessNode: (node) => {
                    return true;
                },
                processNode: processNodeDefinitions.processDefaultNode
            }];

        return htmlToReactParser.parseWithInstructions(viewModel.getLayout().innerHTML, () => {
            return true;
        }, processingInstructions);
    }
}