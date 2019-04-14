import OSCMessage from "../osc/osc-message";
import {default as PluginParameterPrototype} from "../models/plugin-parameter-prototype";
import PluginPrototype from "../models/plugin-prototype";
import {PluginParameterValueType} from "../models/plugin-parameter-model";
import {PluginGetParameters, PluginParameters} from "../api/messages/plugin-messages";

export default class PAGConfigBuilder {
    public buildPluginPrototype(pluginRef: string): Promise<PluginPrototype> {
        let prototype = new PluginPrototype();

        return new Promise((resolve, reject) => {
            window.WOAP.ServiceLocator.getAPIWorker()
                .request(new PluginGetParameters(pluginRef), PluginParameters.getAddress(pluginRef))
                .then((message: OSCMessage) => {
                    let msg = new PluginParameters(pluginRef, message);

                    if (!msg.isValid())
                        return;

                    let paramsInfo = msg.getParameters();
                    for (let paramInfo of paramsInfo) {
                        let paramPrototype = new PluginParameterPrototype(PluginParameterValueType.Float, paramInfo[0], paramInfo[1]);
                        prototype.addParameter(paramPrototype);
                    }

                    resolve(prototype);
                })
                .catch((reason) => reject(reason));
        });
    }
}