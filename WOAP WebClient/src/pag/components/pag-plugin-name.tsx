import * as React from "react";
import * as PropTypes from "prop-types";
import TrackPluginModel from "../../models/track-plugin-model";

interface PAGPluginNameState {

}

interface PAGPluginNameProps {

}

export default class PAGPluginName extends React.Component<PAGPluginNameProps, PAGPluginNameState> {
    static contextTypes = {
        pluginModel: PropTypes.instanceOf(TrackPluginModel)
    };

    public shouldComponentUpdate(props, state, context) {
        return context.pluginModel.getName() !== this.context.pluginModel.getName();
    }

    public render() {
        return (<span className="pagPluginName">{this.context.pluginModel.getName()}</span>);
    }
}
