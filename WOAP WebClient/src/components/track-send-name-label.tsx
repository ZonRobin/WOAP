import * as React from "react";
import Color from "../helpers/color/color";
import NameLabel from "./name-label";
import ColorHelper from "../helpers/color/color-helper";
import {default as TrackSendModel, TrackSendModelListener} from "../models/track-send-model";

interface TrackSendNameLabelProps {
    model: TrackSendModel;
    editable?: boolean;
    maxFontSize?: number;
}

interface TrackSendNameLabelState {

}

export default class TrackSendNameLabel extends React.Component<TrackSendNameLabelProps, TrackSendNameLabelState> implements TrackSendModelListener {
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
        this.props.model.addListener(this);
    }

    public componentWillUnmount() {
        this.props.model.removeListener(this);
    }

    public nameChanged(newName: string): void {
        this.forceUpdate();
    }

    public colorChanged(newColor: Color): void {
        this.forceUpdate();
    }

    public gainChanged(newGain: number): void {
    }

    public panChanged(newPan: number): void {
    }

    public audioSourcePositionChanged(newPosition: number): void {
    }

    public sendWillBeDeleted(): void {
    }

    public shouldComponentUpdate(props: TrackSendNameLabelProps) {
        return this.props.model !== props.model
            || this.props.editable !== props.editable
            || this.props.maxFontSize !== props.maxFontSize;
    }

    public componentWillReceiveProps(newProps: TrackSendNameLabelProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeListener(this);
            newProps.model.addListener(this);
        }
    }

    public textEditedCallback(newText: string): void {
        this.props.model.setName(newText);
    }

    public render() {
        let sendColor = this.props.model.getColor();
        let darkText = ColorHelper.getPerceivedBrightness(sendColor) > 0.5;

        return (
            <div className="trackSendNameLabel">
                <NameLabel
                    text={this.props.model.getName()}
                    backgroundColor={sendColor}
                    useDarkTextColor={darkText}
                    maxFontSize={this.props.maxFontSize}
                    editable={this.props.editable}
                    textEdited={this.textEditedCallbackBound}
                    fitText={!this.props.editable}
                />
            </div>);
    }
}
