//=============================================================================
// <summary>
// FogForceFieldParameter 
// </summary>
// <author>CGC_12_堀 大輔</author>
//=============================================================================
using System;
using System.Collections.Generic;
using System.Numerics;
using via;
using via.attribute;

namespace blackfilter
{
	public enum ForceMode
	{
		ForceMode_Impulse,
		ForceMode_Continious,
		ForceMode_Max,
	
	}

	/// <summary>
	/// フォグフォースフィールドクラス：フォグのインタラクトに使うパラメータを格納したクラス
	/// </summary>
	public class FogForceField : via.Behavior
	{
		[DataMember] private float power;
		[DataMember] private float radius;

        [DataMember] ForceMode forceMode;
		
		public override void awake()
		{


		}

		public override void start()
		{

		}

		public float GetPower()
		{
			return power;
		}
		public float GetRadius()
		{
			return radius;
		}


		public override void update()
		{

		}
	}
}
