//=============================================================================
// <summary>
// HUDFogController 
// </summary>
// <author>CGC_12_堀　大輔</author>
//=============================================================================
using System;
using System.Collections.Generic;
using via;
using via.attribute;
using via.gui;

namespace blackfilter
{
    /// <summary>
    ///HUDフォグコントローラクラス：HUDに表示されるフォグの制御を伝達するクラス。
    /// </summary>
    public class HUDFogController : via.Behavior
    {

        private GUI cpGui;
        private Panel hudFogPanel;  

        public override void awake()
        {
        }

        public override void start()
        {
            base.start();

            cpGui = GameObject.getComponent<GUI>();
            hudFogPanel = cpGui.getObject<Panel>("/PNL_Fog");
           
            
        }

        public override void update()
        {
           
        }

        public void SetPanelColor(Color color,float alpha)
        {
            hudFogPanel.ColorScale = new Float4((float)color.r/255, (float)color.g / 255, (float)color.b / 255, alpha);
        }

     
    }
}
