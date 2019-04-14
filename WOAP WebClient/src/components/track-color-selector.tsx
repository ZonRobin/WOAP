import * as React from "react";
import {
    default as TrackModel,
    TrackModelInfoListener,
} from "../models/track-model";
import Color from "../helpers/color/color";
import ButtonList, {ButtonListItem} from "./button-list";
import {ContextMenu, ContextMenuProvider} from "react-contexify";
import ContextMenuContent from "./context-menu-content";
import {ClickableComponent} from "./clickable-component";

interface TrackColorSelectorProps {
    model: TrackModel;
}

interface TrackColorSelectorState {
}

export default class TrackColorSelector extends ClickableComponent<TrackColorSelectorProps, TrackColorSelectorState> implements TrackModelInfoListener {

    static defaultProps = {
        model: null
    };

    protected colorSelectedBound;

    public constructor(props) {
        super(props);
        this.colorSelectedBound = this.colorSelected.bind(this);
    }

    public componentWillMount() {
        this.props.model.addInfoListener(this);
    }

    public componentWillUnmount() {
        this.props.model.removeInfoListener(this);
    }

    public nameChanged(newName: string): void {
    }

    public colorChanged(newColor: Color): void {
        this.forceUpdate();
    }

    public trackWillBeDeleted(): void {
    }

    public shouldComponentUpdate(props: TrackColorSelectorProps, state: TrackColorSelectorState) {
        return this.props.model !== props.model;
    }

    public componentWillReceiveProps(newProps: TrackColorSelectorProps) {
        if (this.props.model !== newProps.model) {
            this.props.model.removeInfoListener(this);
            newProps.model.addInfoListener(this);
        }
    }

    public colorSelected(id: number) {
        window.WOAP.ContextMenuManager.dismissMenu();

        for (let i = 0; i < this.colors.length; ++i) {
            if (id === this.colors[i][0]) {
                this.props.model.setColor(new Color("#" + this.colors[i][1]));
                return;
            }
        }
    }

    public clicked(): void {
    }

    protected colors: [[number, string]] = [
        [0, "E12525"], // red
        [1, "F5A22E"], // orange
        [2, "F0E223"], // yellow
        [3, "7AD975"], // light green
        [4, "1FBA46"], // green
        [5, "00C1E8"], // light blue
        [6, "4967F2"], // blue
        [7, "7239B8"], // purple
        [8, "808080"], // grey
        [9, "1E1E1E"], // black
        [10, "DCDCDC"] // white
    ];

    public render() {
        return (
            <div className="trackColorSelector">
                <ContextMenuProvider id="colorPalette" event="onClick">
                    <button type="button" className="trackColorSelectButton" {...this.getClickableEventProps()}>
                        Color
                    </button>
                </ContextMenuProvider>
                <ContextMenu id="colorPalette">
                    <ColorList buttons={this.colors} onItemClick={this.colorSelectedBound}/>
                </ContextMenu>
            </div>

        );
    }
}

class ColorList extends ButtonList {
    public render() {
        return (
            <ContextMenuContent className="colorList">
                {this.props.buttons.map((item: [any, string]) =>
                    <ColorListItem key={"item" + item[0]} id={item[0]} text={item[1]}
                                   onClick={this.props.onItemClick}/>
                )}
            </ContextMenuContent>
        );
    }
}

class ColorListItem extends ButtonListItem {
    public render() {
        return (
            <div {...this.getClickableEventProps()}
                 className="colorListItem brightOnActive"
                 style={{backgroundColor: "#" + this.props.text}}
            />);
    }
}