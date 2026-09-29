using UnityEngine;

[ExecuteInEditMode, ImageEffectAllowedInSceneView]
public class RimLight : MonoBehaviour
{
    private Renderer renderer;
    Material _material;
    [SerializeField] private float _rimPower;
    // Start is called before the first frame update
    void Start()
    {
        // 全ての Renderer を取得
        renderer = GetComponent<Renderer>();

        if (renderer!=null)
        {
            // 最初の Renderer の Material をキャッシュ（新規インスタンス）
            _material = new Material(renderer.sharedMaterial);

            // すべての Renderer に同じ Material インスタンスを設定

            renderer.material = _material;
            
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (_material.HasProperty("_RimPower"))
            _material.SetFloat("_RimPower", _rimPower);
    }

    public void setRimPower(float power)
    {
        
        _rimPower = power;
        if (_rimPower < 0) _rimPower = 0;
    }
}
