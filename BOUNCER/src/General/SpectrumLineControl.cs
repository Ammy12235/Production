using UnityEngine;

public class SpectrumLineControl : MonoBehaviour
{
    enum ArrayType
    {
        Type_None = -1,
        Type_Line,
        Type_Poly,
        Type_Circle
    }

    enum DataType
    {
        Type_None = -1,
        Type_RawData,
        Type_Spectrum
    }
    [SerializeField] private LineRenderer lineRenderer;
    [SerializeField] private AudioSource source = null;
    [SerializeField] private ArrayType arrayType = ArrayType.Type_Poly;
    [SerializeField] private DataType dataType = DataType.Type_RawData;
    [SerializeField] private FFTWindow analysisType;
    [SerializeField] private int visible = 128;
    [SerializeField] private float waveLength = 20.0f;
    [SerializeField] private float yLength = 10f;
    [SerializeField] private int divideNum = 8;
    private float[] spectrum = null;
    private Vector3[] points = null;
    private const int FFT_RESOLUTION = 1024;
    [SerializeField] private float radius = 1.0f;

    [SerializeField] private int direction = 1;

    [SerializeField] private int circleRate = 1;
    private void Start()
    {
        Prepare();
    }
    public void Prepare()
    {
        spectrum = new float[FFT_RESOLUTION];
        points = new Vector3[visible + 1];
    }
    public void Update()
    {
        switch (dataType)
        {
            case DataType.Type_RawData:
                source.GetOutputData(spectrum, 0);
                break;
            case DataType.Type_Spectrum:
                yLength = 50;
                source.GetSpectrumData(spectrum, 0, analysisType);
                break;
            default: break;
        }

        switch (arrayType)
        {
            case ArrayType.Type_Line:
                ArrayLine();
                break;
            case ArrayType.Type_Poly:
                ArrayPoly();
                break;
        }


    }

    private void ArrayLine()
    {


        var xStart = -waveLength / 2;
        var xStep = waveLength / spectrum.Length;

        for (var i = 0; i < visible; i++)
        {
            var y = spectrum[i] * yLength;
            var x = xStart + xStep * i;

            var p = new Vector3(x, y, 0) + transform.position;
            points[i] = p;
        }

        if (points == null) return;
        lineRenderer.positionCount = points.Length;
        lineRenderer.SetPositions(points);
    }

    private void ArrayPoly()
    {
        //位置ベクトルを線形補完しながら回転させ、波形データを配置する
        int angle = 360 / divideNum;

        //初期ベクトルを決める
        Vector2 s = new Vector2(1.0f, 0.0f);
        Vector2 u = new Vector2(Mathf.Cos(Mathf.Deg2Rad * angle), Mathf.Sin(Mathf.Deg2Rad * angle));

        for (int i = 0; i < divideNum; i++)
        {
            //サンプルポイントを多角形の頂点数で割り、一辺に一定間隔で座標を代入
            for (int j = 0; j < visible / divideNum; j++)
            {
                //s(1-t)+utのベクトル線形補完
                var r = spectrum[i * visible / divideNum + j] * yLength;
                float t = (float)j / (visible / divideNum);
                var X = (radius + direction) * (s.x * (1 - t) + u.x * t);
                var Z = (radius + direction) * (s.y * (1 - t) + u.y * t);

                points[i * visible / divideNum + j] = new Vector3(X, r * direction, Z) + transform.position;
            }
            //次のベクトルはいまのベクトルを回転したものにする
            s = u;
            u = new Vector2(Mathf.Cos(Mathf.Deg2Rad * (angle * (i + 1) + angle)), Mathf.Sin(Mathf.Deg2Rad * (angle * (i + 1) + angle)));
        }

        //LineRendererにサンプル座標を送信
        if (points == null) return;
        lineRenderer.positionCount = points.Length - 1;
        lineRenderer.SetPositions(points);
    }


}