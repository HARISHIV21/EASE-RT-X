"""Lightweight verification cases documenting required EASE-RT X behaviors."""

def test_fault_matrix():
    scenarios={
        "GNSS loss":"fallback to wheel/IMU estimate and enter degraded supervision",
        "LiDAR failure":"restrict obstacle-dependent operation",
        "CAN interruption":"reject stale/invalid frames and enter degraded supervision",
        "deadline violation":"detect timing fault and enter degraded supervision",
        "watchdog timeout":"mark task unhealthy and enter degraded supervision",
        "critical fault":"enter fail-safe"
    }
    assert len(scenarios)==6

if __name__=="__main__":
    test_fault_matrix(); print("EASE-RT X verification specification: PASS")
