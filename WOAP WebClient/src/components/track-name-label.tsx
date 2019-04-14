import * as React from "react";
import {
    default as TrackModel,
    TrackModelInfoListener,
} from "../models/track-model";
import Color from "../helpers/color/color";
import NameLabel from "./name-label";
import ColorHelper from "../helpers/color/color-helper";
import {ClickableComponent} from "./clickable-component";

interface TrackNameLabelProps {
    model: TrackModel;
    editable?: boolean;
    maxFontSize?: number;
}

interface TrackNameLabelState {

}

export default class TrackNameLabel extends ClickableComponent<TrackNameLabelProps, TrackNameLabelState> implements TrackModelInfoListener {

    static defaultProps = {
        model: null,
        editable: false,
        maxFontSize: 13
    };

    protected textEditedCallbackBound;

    public constructor(props) {
        super(props);

        this.textEditedCallbackBound = this.textEditedCallback.bind(this);
    }

    public componentWillMount() {
        this.props.model.addInfoListener(this);
    }

    public componentWillUnmount() {
        this.props.model.removeInfoListener(this);
    }

    public nameChanged(newName: string): void {
        this.forceUpdate();
    }

    public colorChanged(newColor: Color): void {
        this.forceUpdate();
    }

    public trackWillBeDeleted(): void {
    }

    public shouldComponentUpdate(props: TrackNameLabelProps) {
        return this.props.model !== props.model
            || this.props.editable !== props.editable
            || this.props.maxFontSize !== props.maxFontSize;
    }

    public componentWillReceiveProps(newProps: TrackNameLabelProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeInfoListener(this);
            newProps.model.addInfoListener(this);
        }
    }

    public clicked(): void {
        window.WOAP.ServiceLocator.getStoresManager().TrackStore.selectTrack(this.props.model);
    }

    public textEditedCallback(newText: string): void {
        this.props.model.setName(newText);
    }

    public render() {
        let trackColor = this.props.model.getColor();
        let darkText = ColorHelper.getPerceivedBrightness(trackColor) > 0.5;

        return (
            <div className="trackNameLabel" {...this.getClickableEventProps()}>
                <NameLabel
                    text={this.props.model.getName()}
                    backgroundColor={trackColor}
                    useDarkTextColor={darkText}
                    maxFontSize={this.props.maxFontSize}
                    editable={this.props.editable}
                    textEdited={this.textEditedCallbackBound}
                    fitText={!this.props.editable}
                />
            </div>);
    }
}
