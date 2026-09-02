// SPDX-License-Identifier: Apache-2.0
//
// mrs_viz dashboard client. Plain HTTP polling (see http_server.hpp's
// judgement) — no WebSocket, no framework, no CDN. Observation only: this
// file never sends anything back to the server except GET requests for
// /api/state and /api/config.

(function () {
  "use strict";

  const canvas = document.getElementById("map-canvas");
  const ctx = canvas.getContext("2d");
  const connStatus = document.getElementById("conn-status");
  const stampEl = document.getElementById("stamp");

  let pollPeriodMs = 300; // overwritten by /api/config on load — not hardcoded truth.
  let consecutiveFailures = 0;

  const ROBOT_COLORS = ["#3fa7ff", "#ff9d3f", "#7bd66b", "#e05fd0", "#ffd23f", "#ff5f5f"];

  function colorForRobot(index) {
    return ROBOT_COLORS[index % ROBOT_COLORS.length];
  }

  /// @brief World (x,y) meters -> canvas pixel transform, fit to bounding box
  /// of every point we know about this tick (roadmap nodes + trajectories).
  /// Recomputed every render — this is a diagnostic view, not a fixed map.
  function makeTransform(points, width, height, paddingPx) {
    if (points.length === 0) {
      return { toPx: (x, y) => [width / 2, height / 2], scale: 1 };
    }
    let minX = Infinity, maxX = -Infinity, minY = Infinity, maxY = -Infinity;
    for (const [x, y] of points) {
      if (x < minX) minX = x;
      if (x > maxX) maxX = x;
      if (y < minY) minY = y;
      if (y > maxY) maxY = y;
    }
    const spanX = Math.max(maxX - minX, 1e-6);
    const spanY = Math.max(maxY - minY, 1e-6);
    const usableW = width - 2 * paddingPx;
    const usableH = height - 2 * paddingPx;
    const scale = Math.min(usableW / spanX, usableH / spanY);
    const toPx = (x, y) => {
      const px = paddingPx + (x - minX) * scale;
      // Canvas y grows downward; world y grows "up" on screen by convention.
      const py = height - (paddingPx + (y - minY) * scale);
      return [px, py];
    };
    return { toPx, scale };
  }

  function collectAllPoints(state) {
    const pts = [];
    if (state.roadmap && state.roadmap.have_data) {
      for (const n of state.roadmap.nodes) pts.push([n.x, n.y]);
    }
    if (state.trajectories && state.trajectories.have_data) {
      for (const rt of state.trajectories.robots) {
        for (const p of rt.points) pts.push([p.x, p.y]);
      }
    }
    for (const robot of Object.keys(state.robots || {})) {
      const r = state.robots[robot];
      if (r.has_odom) pts.push([r.x, r.y]);
    }
    return pts;
  }

  function drawRoadmap(state, toPx) {
    if (!state.roadmap || !state.roadmap.have_data) return;
    ctx.strokeStyle = "#3a4a5c";
    ctx.lineWidth = 1.5;
    const byId = {};
    for (const n of state.roadmap.nodes) byId[n.id] = n;
    for (const e of state.roadmap.edges) {
      const a = byId[e.from_id];
      const b = byId[e.to_id];
      if (!a || !b) continue; // 좌표를 지어내지 않는다 — 못 찾으면 안 그린다.
      const [ax, ay] = toPx(a.x, a.y);
      const [bx, by] = toPx(b.x, b.y);
      ctx.beginPath();
      ctx.moveTo(ax, ay);
      ctx.lineTo(bx, by);
      ctx.stroke();
    }
    for (const n of state.roadmap.nodes) {
      const [px, py] = toPx(n.x, n.y);
      ctx.beginPath();
      ctx.arc(px, py, n.is_endpoint ? 6 : 3.5, 0, 2 * Math.PI);
      ctx.fillStyle = n.is_endpoint ? "#ff9d3f" : "#5a7a9c";
      ctx.fill();
    }
  }

  /// @brief Trajectory path + a translucent "tube" of width
  /// `state.trajectories.min_separation_m` (derived from
  /// `mrs.trajectories.limits.min_separation_m` — not a new field, a
  /// rendering choice on an existing one; see dashboard_state.hpp).
  function drawTrajectories(state, toPx, scale, robotIndex) {
    if (!state.trajectories || !state.trajectories.have_data) return;
    const tubeWidthPx = Math.max(state.trajectories.min_separation_m * scale, 1);
    for (const rt of state.trajectories.robots) {
      if (rt.points.length < 2) continue;
      const idx = robotIndex(rt.robot);
      const color = colorForRobot(idx);
      ctx.beginPath();
      ctx.strokeStyle = color;
      ctx.globalAlpha = 0.18;
      ctx.lineWidth = tubeWidthPx;
      ctx.lineCap = "round";
      ctx.lineJoin = "round";
      let [px0, py0] = toPx(rt.points[0].x, rt.points[0].y);
      ctx.moveTo(px0, py0);
      for (let i = 1; i < rt.points.length; i++) {
        const [px, py] = toPx(rt.points[i].x, rt.points[i].y);
        ctx.lineTo(px, py);
      }
      ctx.stroke();
      ctx.globalAlpha = 1.0;

      ctx.beginPath();
      ctx.strokeStyle = color;
      ctx.lineWidth = 1.5;
      ctx.moveTo(px0, py0);
      for (let i = 1; i < rt.points.length; i++) {
        const [px, py] = toPx(rt.points[i].x, rt.points[i].y);
        ctx.lineTo(px, py);
      }
      ctx.stroke();
    }
  }

  function drawRobots(state, toPx, robotIndex) {
    for (const robot of Object.keys(state.robots || {})) {
      const r = state.robots[robot];
      if (!r.has_odom) continue;
      const [px, py] = toPx(r.x, r.y);
      const color = colorForRobot(robotIndex(robot));
      ctx.beginPath();
      ctx.arc(px, py, 9, 0, 2 * Math.PI);
      ctx.fillStyle = color;
      ctx.fill();
      ctx.strokeStyle = "#0a0d12";
      ctx.lineWidth = 1.5;
      ctx.stroke();

      const headingLenPx = 18;
      const hx = px + headingLenPx * Math.cos(-r.yaw);
      const hy = py + headingLenPx * Math.sin(-r.yaw);
      ctx.beginPath();
      ctx.moveTo(px, py);
      ctx.lineTo(hx, hy);
      ctx.strokeStyle = "#0a0d12";
      ctx.lineWidth = 2;
      ctx.stroke();

      ctx.fillStyle = "#e6e6e6";
      ctx.font = "11px sans-serif";
      ctx.fillText(robot, px + 12, py - 10);
    }
  }

  function render(state) {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    const points = collectAllPoints(state);
    const { toPx, scale } = makeTransform(points, canvas.width, canvas.height, 40);
    const robotOrder = Object.keys(state.robots || {});
    const robotIndex = (name) => Math.max(robotOrder.indexOf(name), 0);

    drawRoadmap(state, toPx);
    drawTrajectories(state, toPx, scale, robotIndex);
    drawRobots(state, toPx, robotIndex);

    renderSidePanel(state);
  }

  function renderSidePanel(state) {
    stampEl.textContent = "t=" + (state.stamp_sec || 0).toFixed(2) + "s (ROS clock)";

    const robotsBody = document.querySelector("#robots-table tbody");
    robotsBody.innerHTML = "";
    for (const robot of Object.keys(state.robots || {})) {
      const r = state.robots[robot];
      const tr = document.createElement("tr");
      const pose = r.has_odom
        ? `(${r.x.toFixed(2)}, ${r.y.toFixed(2)}, ${(r.yaw * 180 / Math.PI).toFixed(0)}°)`
        : "no odom yet";
      const rel = r.last_segment_release_seq !== undefined ? r.last_segment_release_seq : "-";
      tr.innerHTML = `<td>${robot}</td><td>${pose}</td><td>release_seq=${rel}</td>`;
      robotsBody.appendChild(tr);
    }

    const trajEl = document.getElementById("traj-summary");
    if (state.trajectories && state.trajectories.have_data) {
      const rows = state.trajectories.robots
        .map((rt) => `${rt.robot}: ${rt.n_points_total} pts`)
        .join(" · ");
      trajEl.innerHTML = `instance=${state.trajectories.instance_id} · min_separation_m=${state.trajectories.min_separation_m} · ${rows}`;
    } else {
      trajEl.innerHTML = '<span class="no-data">no data yet</span>';
    }

    const sadgEl = document.getElementById("sadg-summary");
    const depsBody = document.querySelector("#dependencies-table tbody");
    depsBody.innerHTML = "";
    if (state.execution_constraints && state.execution_constraints.have_data) {
      const ec = state.execution_constraints;
      sadgEl.innerHTML = `commit_seq=${ec.commit_seq} · segments=${ec.n_segments} · dependencies=${ec.n_dependencies_total}${ec.dependencies_truncated ? " (truncated in display)" : ""} · switch_groups=${ec.switch_groups.length}`;
      for (const d of ec.dependencies) {
        const tr = document.createElement("tr");
        tr.innerHTML = `<td>${d.from_id}</td><td>&rarr;</td><td>${d.to_id}</td><td>${d.type}</td>`;
        depsBody.appendChild(tr);
      }
    } else {
      sadgEl.innerHTML = '<span class="no-data">no data yet</span>';
    }

    const replanEl = document.getElementById("replan-summary");
    if (state.replan_status && state.replan_status.have_data) {
      if (state.replan_status.goals.length === 0) {
        replanEl.innerHTML = '<span class="no-data">action status topic live, 0 goals observed so far</span>';
      } else {
        replanEl.innerHTML = state.replan_status.goals
          .map((g) => `${g.goal_id_short}: ${g.status}`)
          .join("<br/>");
      }
    } else {
      replanEl.innerHTML = '<span class="no-data">no data yet (no goal has been sent to the replan action)</span>';
    }

    const subgoalEl = document.getElementById("subgoal-summary");
    subgoalEl.innerHTML = `<span class="no-data">${(state.subgoal_stream && state.subgoal_stream.note) || "no data"}</span>`;

    const diagEl = document.getElementById("diag-summary");
    diagEl.innerHTML = `<span class="no-data">${(state.diagnostics && state.diagnostics.note) || "no data"}</span>`;

    const resultEl = document.getElementById("result-summary");
    if (state.result_summary && state.result_summary.have_data) {
      resultEl.textContent = JSON.stringify(state.result_summary.content);
    } else {
      resultEl.innerHTML = '<span class="no-data">no result.json configured/found</span>';
    }
  }

  function setConnStatus(ok, message) {
    connStatus.className = "badge " + (ok ? "badge-ok" : "badge-error");
    connStatus.textContent = message;
  }

  async function pollOnce() {
    try {
      const resp = await fetch("/api/state", { cache: "no-store" });
      if (!resp.ok) throw new Error("HTTP " + resp.status);
      const state = await resp.json();
      render(state);
      consecutiveFailures = 0;
      setConnStatus(true, "live (polling every " + pollPeriodMs + " ms)");
    } catch (err) {
      consecutiveFailures += 1;
      setConnStatus(false, "poll failed x" + consecutiveFailures + ": " + err.message);
    }
  }

  async function init() {
    try {
      const resp = await fetch("/api/config", { cache: "no-store" });
      const cfg = await resp.json();
      pollPeriodMs = cfg.poll_period_ms || pollPeriodMs;
    } catch (err) {
      // config fetch failing is not fatal — fall back to the default period.
    }
    pollOnce();
    setInterval(pollOnce, pollPeriodMs);
  }

  init();
})();
