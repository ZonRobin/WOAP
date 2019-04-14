import * as React from "react";
import Draggable from "react-draggable";
import {Utility} from "../utility";

interface ModalState {

}

interface ModalProps {
    content: React.ReactElement<any>;
    id: string;
}

export default class Modal extends React.Component<ModalProps, ModalState> {
    static defaultProps = {
        content: <div/>,
        id: ""
    };

    protected mouseDownBound;
    protected dragStopBound;
    protected defaultPosition;

    public constructor(props) {
        super(props);

        this.mouseDownBound = this.mouseDown.bind(this);
        this.dragStopBound = this.dragStop.bind(this);

        let windowSize = Utility.getWindowSize();

        let storedPos = Utility.getLocalStorageValueUpdatingTimestamp("pos-" + this.props.id);
        if (storedPos.length !== 0) {
            let parsed = JSON.parse(storedPos);
            this.defaultPosition = {x: parsed[0], y: parsed[1]};
        } else {
            let x = windowSize[0] > 700 ? 100 : 0;
            let y = windowSize[1] > 700 ? 30 : 0;
            this.defaultPosition = {x: x, y: y};
        }
    }

    public mouseDown() {
        window.WOAP.ServiceLocator.getModalManager().setModalOnTop(this.props.id);
    }

    public dragStop(ev, data) {
        Utility.setLocalStorageValueWithTimestamp("pos-" + this.props.id, JSON.stringify([data.x, data.y]));
    }

    public render() {
        return (
            <Draggable handle=".dragHandle"
                       bounds="parent"
                       onMouseDown={this.mouseDownBound}
                       onStop={this.dragStopBound}
                       defaultPosition={this.defaultPosition}
            >
                <div className="modalContainer"
                     tabIndex={-1}
                >
                    {this.props.content}
                </div>
            </Draggable>
        );
    }
}
