import TrackPluginModel from "../models/track-plugin-model";
import PluginParameterModel from "../models/plugin-parameter-model";
import PluginViewModel from "../models/plugin-view-model";

export default class PAGViewBuilder
{
    public buildDefaultView(pluginModel: TrackPluginModel): void
    {
        let parameterLayouts = '';
        for (let parameter of pluginModel.getParameters())
        {
            parameterLayouts += this.buildParameterLayout(parameter);
        }

        let layout = '<!--suppress ALL -->' +
            '<view default="default" lang="pag-jsx">' +
                '<div class="pagDefaultView">' +
                    parameterLayouts +
                '</div>' +
            '</view>';

        let layoutParser = new DOMParser();
        let parsedLayout = layoutParser.parseFromString(layout, "text/xml");

        let viewModel = new PluginViewModel();
        viewModel.setIsDefault(true);
        viewModel.setLayoutJSX(parsedLayout.getElementsByTagName('view')[0]);
        viewModel.setName('defaultView');

        pluginModel.addView(viewModel);
    }

    protected buildParameterLayout(pluginParameterModel: PluginParameterModel): string
    {
        return '' +
            '<div class="pagDefaultParameterContainer">' +
                '<PAGParameter ref="' + pluginParameterModel.getRef() + '">' +
                    '<div class="pagDefaultParameterNameContainer">' +
                        '<PAGParameterName />' +
                    '</div>' +
                    '<div class="pagDefaultParameterControlContainer">' +
                        '<div class="pagDefaultParameterControl">' +
                            '<PAGRotarySlider />' +
                        '</div>' +
                    '</div>' +
                    '<div class="pagDefaultParameterValueContainer">' +
                        '<PAGParameterValue />' +
                    '</div>' +
                '</PAGParameter>' +
            '</div>';
    }
}