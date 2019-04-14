import * as React from "react";
import PAGPlugin from "./components/pag-plugin";
import PAGConfigParserXML from "./pag-config-parser";
import PluginViewPrototype from "../models/plugin-view-model";
import PAGViewCompiler from "./pag-view-compiler";
import {UUID} from "../utility";
import TrackPluginModel from "../models/track-plugin-model";
import PluginPrototype from "../models/plugin-prototype";
import PAGViewBuilder from "./pag-view-builder";
import {ConfigurationManager} from "../configuration-manager";

export default class PAGManager {
    protected configParser: PAGConfigParserXML = new PAGConfigParserXML();
    protected viewCompiler: PAGViewCompiler = new PAGViewCompiler();
    protected viewBuilder: PAGViewBuilder = new PAGViewBuilder();

    public compileView(viewPrototype: PluginViewPrototype, pluginModel: TrackPluginModel): React.ReactElement<any> {
        return this.viewCompiler.compile(viewPrototype, pluginModel);
    }

    public prepareModelToPresent(pluginModel: TrackPluginModel): Promise<void> {
        return new Promise<void>((resolve, reject) => {
            window.WOAP.ServiceLocator.getStoresManager().PluginStore.getPluginPrototype(pluginModel.getTypeId(), this.configParser).then((prototype: PluginPrototype) => {
                for (let view of prototype.getViews()) {
                    pluginModel.addView(view);
                }
                for (let image of prototype.getImages()) {
                    pluginModel.addImage(image);
                }
                pluginModel.setScript(prototype.getScript());

                if (prototype.getParameters().length !== 0) {
                    if (prototype.getParameters().length !== pluginModel.getParameters().length) {
                        throw new Error("Number of parameters in " + pluginModel.getRef() + " doesn\"t fit the real number");
                    }

                    for (let prototypeParam of prototype.getParameters()) {
                        let modelParam = pluginModel.getParameterUnchecked(prototypeParam.getIndex());

                        modelParam.setName(prototypeParam.getName());
                        modelParam.setValueType(prototypeParam.getValueType());
                        modelParam.setSteps(prototypeParam.getSteps());
                    }
                }
                resolve();
            }).catch((reason) => {
                if (ConfigurationManager.isDebug()) {
                    console.error("Error loading " + pluginModel.getRef() + " plugin config, reason: " + reason);
                }
                this.viewBuilder.buildDefaultView(pluginModel);
                resolve();
            });
        });
    }

    public getPluginById(trackId: UUID, pluginId: UUID) {
        return window.WOAP.ServiceLocator.getStoresManager().TrackStore.getTrackById(trackId).getPluginById(pluginId);
    }

    public openPluginModal(trackPluginModel: TrackPluginModel) {
        // fetch parameters before creating plugin

        window.WOAP.ServiceLocator.getModalManager().openModalIfNotExists(
            React.createElement(PAGPlugin, {
                pluginModel: trackPluginModel,
                closeCallback: () => this.closePluginModal(trackPluginModel)
            }),
            "pag" + trackPluginModel.getId()
        );
    }

    public closePluginModal(trackPluginModel: TrackPluginModel) {
        window.WOAP.ServiceLocator.getModalManager().closeModal("pag" + trackPluginModel.getId());
    }
}
