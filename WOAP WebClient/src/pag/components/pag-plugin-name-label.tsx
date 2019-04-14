import * as React from "react";
import {default as TrackPluginModel, TrackPluginModelListener} from "../../models/track-plugin-model";
import NameLabel from "../../components/name-label";
import {TrackModelInfoListener} from "../../models/track-model";
import Color from "../../helpers/color/color";

interface PAGPluginNameLabelProps {
    model: TrackPluginModel;
}

interface PAGPluginNameLabelState {

}

export default class PAGPluginNameLabel extends React.Component<PAGPluginNameLabelProps, PAGPluginNameLabelState> implements TrackPluginModelListener, TrackModelInfoListener {
    static defaultProps = {
        model: null
    };

    protected textEditedCallbackBound;

    public constructor(props) {
        super(props);

        this.textEditedCallbackBound = this.textEditedCallback.bind(this);
    }

    public componentWillMount() {
        this.props.model.addListener(this);
        this.props.model.getTrack().addInfoListener(this);
    }

    public componentWillUnmount() {
        this.props.model.removeListener(this);
        this.props.model.getTrack().removeInfoListener(this);
    }

    public nameChanged(newName: string): void {
        this.forceUpdate();
    }

    public bypassChanged(newState: boolean): void {
    }

    public activeChanged(newState: boolean): void {
    }

    public colorChanged(newColor: Color): void {
    }

    public pluginWillBeDeleted(): void {
    }

    public trackWillBeDeleted(): void {
    }

    public shouldComponentUpdate(props: PAGPluginNameLabelProps) {
        return this.props.model !== props.model;
    }

    public componentWillReceiveProps(newProps: PAGPluginNameLabelProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeListener(this);
            this.props.model.getTrack().removeInfoListener(this);
            newProps.model.addListener(this);
            newProps.model.getTrack().addInfoListener(this);
        }
    }

    public textEditedCallback(newText: string): void {
        this.props.model.setName(newText);
    }

    public render() {
        return (
            <div className="pagPluginName dragHandle">
                <NameLabel
                    text={this.props.model.getTrack().getName() + " - " + this.props.model.getName()}
                    editorText={this.props.model.getName()}
                    editable={true}
                    textEdited={this.textEditedCallbackBound}
                    fitText={false}
                />
            </div>);
    }
}
