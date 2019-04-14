import * as React from "react";
import PNGImage from "../../components/png-image";

interface PAGImageState {
}

interface PAGImageProps {
    image: PNGImage
}

export default class PAGImage extends React.Component<PAGImageProps, PAGImageState> {
    public render() {
        return <img
            src={this.props.image.getHTMLData()}
            alt={"Plugin " + this.props.image.getRef() + " image"}
        />;
    }
}