import * as React from "react";
import * as PropTypes from "prop-types";
import TrackPluginModel from "../../models/track-plugin-model";

interface PAGPluginIdState {

}

interface PAGPluginIdProps {

}

export default class PAGPluginId extends React.Component<PAGPluginIdProps, PAGPluginIdState> {
    static contextTypes = {
        pluginModel: PropTypes.instanceOf(TrackPluginModel)
    };

    public shouldComponentUpdate(props, state, context) {
        return context.pluginModel.getId() !== this.context.pluginModel.getId();
    }

    public render() {
        return (<span className="pagPluginId">{this.context.pluginModel.getId()}</span>);
    }
}
